import pygame
import sys
import os
from abc import ABC, abstractmethod

# --- ФУНКЦИЯ ДЛЯ ПУТЕЙ (ДЛЯ EXE) ---
def resource_path(relative_path):
    """ Получает абсолютный путь к ресурсу, работает для dev и для PyInstaller """
    try:
        # PyInstaller создает временную папку _MEIPASS
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")
    return os.path.join(base_path, relative_path)

# Папка со скриптом (для поиска ассетов при обычном запуске)
ASSETS_PATH = os.path.dirname(os.path.abspath(__file__))

# --- ИНТЕРФЕЙС НАБЛЮДАТЕЛЯ ---
class Observer(ABC):
    @abstractmethod
    def update(self, subject):
        pass

# --- СУБЪЕКТ (Villager) ---
class Villager:
    def __init__(self, hp: int = 20):
        self._observers = []
        self.emeralds = 0
        self._is_hit = False
        self.hpBar = hp

    def attach(self, observer: Observer):
        if observer not in self._observers: 
            self._observers.append(observer)

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

# --- НАБЛЮДАТЕЛЬ 1: ТОРГОВЫЙ СЛОТ ---
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

# --- НАБЛЮДАТЕЛЬ 2: ГРАФИЧЕСКИЙ ДВИЖОК ---
class Game(Observer):
    def __init__(self):
        pygame.init()
        self.WIDTH, self.HEIGHT = 700, 500
        self.screen = pygame.display.set_mode((self.WIDTH, self.HEIGHT))
        pygame.display.set_caption("Minecraft Villager Shop")
        self.clock = pygame.time.Clock()
        self.font = pygame.font.SysFont("Arial", 22, bold=True)
        self.game_over = False

        # Переменные для эффекта покупки
        self.purchase_message = ""
        self.show_purchase_effect = 0 

        self._load_assets()

        self.center_pos = (self.WIDTH // 2, self.HEIGHT // 2 + 40)
        self.current_mood = self.mood_icons["Neutral"]

        self.trades = [
            TradeSlot("Хлеб", 1),
            TradeSlot("Каменный меч", 4),
            TradeSlot("Железный нагрудник", 12),
            TradeSlot("Алмазная кирка", 20)
        ]

        self.villager = Villager()
        self.villager.attach(self)
        for t in self.trades: 
            self.villager.attach(t)

        self.active_index = 0
        self.show_item = False 

    def _load_assets(self):
        def load_scaled(name, w, h):
            # Используем resource_path для совместимости с EXE
            path = resource_path(name)
            if not os.path.exists(path):
                # Если файл потерялся, создаем заглушку
                surf = pygame.Surface((w, h))
                surf.fill((255, 0, 255))
                return surf
            img = pygame.image.load(path).convert_alpha()
            return pygame.transform.scale(img, (w, h))

        try:
            self.img_bg = load_scaled("background.jpg", self.WIDTH, self.HEIGHT)
            self.img_death_bg = load_scaled("dead.png", self.WIDTH, self.HEIGHT)
            self.img_villager = load_scaled("villager_base.png", 140, 240)
            self.img_golem = load_scaled("golem.png", 200, 300)
            self.img_stars = load_scaled("stars.png", 100, 100) # Загрузка звезд
            
            self.item_images = {
                "Хлеб": load_scaled("bread.png", 140, 240),
                "Каменный меч": load_scaled("sword.png", 140, 240),
                "Железный нагрудник": load_scaled("chest.png", 140, 240),
                "Алмазная кирка": load_scaled("pixaxe.png", 140, 240)
            }
            self.mood_icons = {
                "Neutral": load_scaled("mood_neutral.png", 80, 80),
                "Angry": load_scaled("mood_angry.png", 80, 80),
                "Interested": load_scaled("mood_interested.png", 80, 80)
            }
        except Exception as e:
            print(f"Критическая ошибка загрузки: {e}")

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
            if event.type == pygame.QUIT: 
                pygame.quit(); sys.exit()
            
            if self.game_over:
                if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                    pygame.quit(); sys.exit()
                continue
            
            if event.type == pygame.MOUSEBUTTONDOWN:
                rect = self.img_villager.get_rect(center=self.center_pos)
                if rect.collidepoint(event.pos):
                    if event.button == 1: self.villager.hit()
                    elif event.button == 3: self.villager.offerEmeralds(self.villager.emeralds + 1)

            if event.type == pygame.KEYDOWN:
                if event.key in [pygame.K_1, pygame.K_2, pygame.K_3, pygame.K_4]:
                    self.active_index = event.key - pygame.K_1
                    self.show_item = True
                
                if event.key == pygame.K_ESCAPE or event.key == pygame.K_TAB:
                    self.show_item = False
                
                if event.key == pygame.K_SPACE:
                    active_trade = self.trades[self.active_index]
                    if active_trade.buy(self.villager):
                        # Активируем эффект покупки
                        self.purchase_message = f"Куплено: {active_trade.itemName}"
                        self.show_purchase_effect = 90

    def draw(self):
        if not self.game_over:
            self.screen.blit(self.img_bg, (0, 0))
            active_trade = self.trades[self.active_index]
            
            # Отрисовка основного объекта (житель или предмет)
            current_img = self.item_images[active_trade.itemName] if self.show_item else self.img_villager
            img_rect = current_img.get_rect(center=self.center_pos)
            self.screen.blit(current_img, img_rect)

            # Отрисовка настроения
            mood_rect = self.current_mood.get_rect(center=(self.center_pos[0], img_rect.top - 45))
            self.screen.blit(self.current_mood, mood_rect)

            if self.show_purchase_effect > 0:
                stars_rect = self.img_stars.get_rect(center=(self.center_pos[0], img_rect.top - 80))
                self.screen.blit(self.img_stars, stars_rect)
                
                msg_surf = self.font.render(self.purchase_message, True, (0, 255, 0))
                self.screen.blit(msg_surf, (self.WIDTH//2 - msg_surf.get_width()//2, 120))
                self.show_purchase_effect -= 1
            
            # Интерфейс
            self.screen.blit(self.font.render(f"HP: {self.villager.hpBar}", True, (200,0,0)), (20,20))
            self.screen.blit(self.font.render(f"Изумруды: {self.villager.emeralds}", True, (255,255,255)), (20,55))
            
            status = f"ВЫБРАНО: {active_trade.itemName} | Цена: {active_trade.get_current_price()}"
            info_text = self.font.render(status, True, (255, 255, 0))
            self.screen.blit(info_text, (self.WIDTH - info_text.get_width() - 20, 20))
            
            hint = "ЛКМ: Удар | ПКМ: Монета | 1-4: Товар | ПРОБЕЛ: Купить"
            hint_surf = self.font.render(hint, True, (255,255,255))
            self.screen.blit(hint_surf, (self.WIDTH//2 - hint_surf.get_width()//2, self.HEIGHT-40))
        else:
            self.screen.blit(self.img_death_bg, (0, 0))
            golem_rect = self.img_golem.get_rect(center=(self.WIDTH * 3 // 4, self.HEIGHT//2))
            self.screen.blit(self.img_golem, golem_rect)
            msg = self.font.render("ЖИТЕЛЬ ПОГИБ. ESC ДЛЯ ВЫХОДА", True, (255, 255, 255))
            self.screen.blit(msg, (self.WIDTH//2 - msg.get_width()//2, self.HEIGHT - 40))

        pygame.display.flip()

    def run(self):
        while True:
            self.handle_input()
            self.draw()
            self.clock.tick(60)

if __name__ == "__main__":
    Game().run()