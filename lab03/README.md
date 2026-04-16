Описание: игрок находит жителя, с которым хочет поторговаться, потому что после вылазки в пещеру он нашёл энное количество изумрудов. Игрок предлагает жителю изумруды либо случайно бьёт его (бывает), а житель просто «орёт» (оповещает) о том, что произошло событие. Наблюдатели, подписанные на жителя, улавливают этот сигнал и реагируют в зависимости от того, что именно изменилось в состоянии субъекта.

Субъект Villager
Хранит данные о накопленных изумрудах (emeralds), текущем здоровье (hpbar) и флаге получения урона (isHit). Содержит список объектов, реализующих интерфейс Observer. Предоставляет методы attach для регистрации новых наблюдателей и detach для их удаления из списка рассылки. При взаимодействии (методы offerEmeralds, takeEmeralds или hit) объект автоматически вызывает внутренний метод notify(). Этот метод по цепочке запускает update() у всех зарегистрированных наблюдателей, передавая им актуальную информацию о состоянии жителя.

Интерфейс Observer
Определяет единый стандарт для всех компонентов, желающих реагировать на действия жителя. Содержит абстрактный метод update(subject: Villager), который гарантирует, что любой наблюдатель сможет получить ссылку на объект жителя и проанализировать его параметры.

Конкретные наблюдатели
TradeSlot (Торговый слот): Отвечает за экономику. Этот наблюдатель внимательно следит за статусом isHit. Если он видит, что жителя ударили, класс увеличивает множитель цены priceMultiplier. Таким образом, игрок наказывается за агрессию — товары в слотах становятся дороже.
Game: Выступает в роли главного контроллера настроения и визуализации.
Реакция: В методе update анализирует состояние жителя: если isHit — меняет иконку настроения на «Angry», если emeralds > 0 — на «Interested».
Состояние: Также следит за hpbar — если здоровье падает до нуля, переключает флаг game_over, останавливая торговлю и вызывая экран с Големом.
IronGolem: Наблюдатель-защитник. Пока житель в порядке, голем пассивен, но через интерфейс Observer он готов мгновенно получить сигнал о критическом состоянии жителя для активации своей логики.

<img width="771" height="971" alt="l3" src="https://github.com/user-attachments/assets/615099fb-df55-4b0a-9392-8926113376de" />



Код с паттерном

```py
import pygame
import sys
import os
from abc import ABC, abstractmethod

def resource_path(relative_path):
    try:
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")
    return os.path.join(base_path, relative_path)

ASSETS_PATH = os.path.dirname(os.path.abspath(__file__))

# интерфейс наблюдателя
class Observer(ABC):
    @abstractmethod
    def update(self, subject):
        pass

class Villager:
    def __init__(self, hp: int = 20):
        self._observers = []
        self.emeralds = 0
        self._is_hit = False
        self.hpBar = hp

    def attach(self, observer: Observer):
        if observer not in self._observers: self._observers.append(observer)

    def offerEmeralds(self, count: int):
        self.emeralds = count
        self.notify()

    def takeEmeralds(self, count: int) -> bool:
        if self.emeralds >= count:
            self.emeralds -= count
            self.notify()
            return True
        return False

    def hit(self):
        if self.hpBar > 0:
            self._is_hit = True
            self.hpBar -= 2
            self.notify()
            self._is_hit = False

    def notify(self):
        for observer in self._observers:
            observer.update(self)

    def getEmeralds(self) -> int: return self.emeralds
    def isHit(self) -> bool: return self._is_hit

class TradeSlot(Observer):
    def __init__(self, itemName: str, basePrice: int):
        self.itemName = itemName
        self.basePrice = basePrice
        self.priceMultiplier = 1

    def update(self, subject: Villager):
        if subject.isHit():
            self.priceMultiplier += 1

    def get_current_price(self):
        return self.basePrice * self.priceMultiplier

    def buy(self, subject: Villager):
        return subject.takeEmeralds(self.get_current_price())

class IronGolem(Observer):
    def update(self, subject: Villager):
        pass

class Game(Observer):
    def __init__(self):
        pygame.init()
        self.WIDTH, self.HEIGHT = 700, 500
        self.screen = pygame.display.set_mode((self.WIDTH, self.HEIGHT))
        pygame.display.set_caption("Minecraft Villager Shop")
        self.clock = pygame.time.Clock()
        self.font = pygame.font.SysFont("Arial", 22, bold=True)
        self.death_font = pygame.font.SysFont("Arial", 40, bold=True)
        self.game_over = False

        try:
            def load_scaled(name, w, h):
                path = os.path.join(ASSETS_PATH, name)
                img = pygame.image.load(path).convert_alpha()
                return pygame.transform.scale(img, (w, h))

            self.img_bg = load_scaled("background.jpg", self.WIDTH, self.HEIGHT)
            self.img_death_bg = load_scaled("dead.png", self.WIDTH, self.HEIGHT)

            self.img_villager = load_scaled("villager_base.png", 140, 240)
            
            self.item_images = {
                "Хлеб": load_scaled("bread.png", 140, 240),
                "Каменный меч": load_scaled("sword.png", 140, 240),
                "Железный нагрудник": load_scaled("chest.png", 140, 240),
                "Алмазная кирка": load_scaled("pixaxe.png", 140, 240)
            }

            try:
                self.img_golem = load_scaled("golem.png", 200, 300)
            except:
                self.img_golem = pygame.Surface((200, 300)); self.img_golem.fill((150, 0, 0))

            self.mood_icons = {
                "Neutral": load_scaled("mood_neutral.png", 80, 80),
                "Angry": load_scaled("mood_angry.png", 80, 80),
                "Interested": load_scaled("mood_interested.png", 80, 80)
            }
        except Exception as e:
            print(f"Ошибка ассетов: {e}"); sys.exit()

        self.center_pos = (self.WIDTH // 2, self.HEIGHT // 2 + 40)
        self.current_mood = self.mood_icons["Neutral"]

        self.trades = [
            TradeSlot("Хлеб", 1),
            TradeSlot("Каменный меч", 4),
            TradeSlot("Железный нагрудник", 12),
            TradeSlot("Алмазная кирка", 20)
        ]
        self.active_index = 0
        self.show_item = False 

        self.villager = Villager()
        self.villager.attach(self)
        for t in self.trades: self.villager.attach(t)

    def update(self, subject: Villager):
        if subject.hpBar <= 0:
            self.game_over = True
        elif subject.isHit():
            self.current_mood = self.mood_icons["Angry"]
        elif subject.getEmeralds() > 0:
            self.current_mood = self.mood_icons["Interested"]
        else:
            self.current_mood = self.mood_icons["Neutral"]

    def handle_input(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT: pygame.quit(); sys.exit()
            if self.game_over: continue
            
            if event.type == pygame.MOUSEBUTTONDOWN:
                rect = self.img_villager.get_rect(center=self.center_pos)
                if rect.collidepoint(event.pos):
                    if event.button == 1: # ЛКМ - Удар
                        self.villager.hit()
                    elif event.button == 3: # ПКМ - Изумруды
                        self.villager.offerEmeralds(self.villager.emeralds + 1)

            if event.type == pygame.KEYDOWN:
                if event.key in [pygame.K_1, pygame.K_2, pygame.K_3, pygame.K_4]:
                    self.active_index = event.key - pygame.K_1
                    self.show_item = True
                
                if event.key == pygame.K_ESCAPE or event.key == pygame.K_TAB:
                    self.show_item = False
                
                if event.key == pygame.K_SPACE:
                    if self.trades[self.active_index].buy(self.villager):
                        print(f"Куплено: {self.trades[self.active_index].itemName}")

    def draw(self):
        if not self.game_over:
            self.screen.blit(self.img_bg, (0, 0))
            
            active_trade = self.trades[self.active_index]
            
            if self.show_item:
                current_img = self.item_images[active_trade.itemName]
                img_rect = current_img.get_rect(center=self.center_pos)
                self.screen.blit(current_img, img_rect)
            else:
                img_rect = self.img_villager.get_rect(center=self.center_pos)
                self.screen.blit(self.img_villager, img_rect)

            mood_rect = self.current_mood.get_rect(center=(self.center_pos[0], img_rect.top - 45))
            self.screen.blit(self.current_mood, mood_rect)
            
            self.screen.blit(self.font.render(f"HP: {self.villager.hpBar}", True, (200,0,0)), (20,20))
            self.screen.blit(self.font.render(f"Изумруды: {self.villager.emeralds}", True, (255,255,255)), (20,55))
            
            status = f"ВЫБРАНО: {active_trade.itemName} | Цена: {active_trade.get_current_price()}"
            info_text = self.font.render(status, True, (255, 255, 0))
            self.screen.blit(info_text, (self.WIDTH - info_text.get_width() - 20, 20))
            
            hint = "ЛКМ: Удар | ПКМ: Монета | 1-4: Выбор | ПРОБЕЛ: Купить"
            hint_surf = self.font.render(hint, True, (255,255,255))
            self.screen.blit(hint_surf, (self.WIDTH//2 - hint_surf.get_width()//2, self.HEIGHT-40))
        else:
            self.screen.blit(self.img_death_bg, (0, 0))
            
            golem_rect = self.img_golem.get_rect(center=(self.WIDTH * 3 // 4, self.HEIGHT//2))
            self.screen.blit(self.img_golem, golem_rect)
            
            hint_exit = self.font.render("Нажми Esc для выхода", True, (255, 255, 255))
            self.screen.blit(hint_exit, (self.WIDTH//2 - hint_exit.get_width()//2, self.HEIGHT - 40))

        pygame.display.flip()

    def run(self):
        while True:
            if self.game_over:
                for event in pygame.event.get():
                    if event.type == pygame.QUIT: pygame.quit(); sys.exit()
                    if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                        pygame.quit(); sys.exit()
            
            self.handle_input()
            self.draw()
            self.clock.tick(60)

if __name__ == "__main__":
    Game().run()
```
