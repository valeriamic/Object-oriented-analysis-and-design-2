import pygame
import sys
import os

# --- КОНФИГУРАЦИЯ ---
# Все файлы ищутся в той же папке, где лежит этот скрипт
BASE_PATH = os.path.dirname(os.path.abspath(__file__))

# --- ТОРГОВЫЙ СЛОТ (Теперь обычный класс данных) ---
class TradeSlot:
    def __init__(self, itemName: str, basePrice: int):
        self.itemName = itemName
        self.basePrice = basePrice
        self.priceMultiplier = 1

    def increase_price(self):
        """Прямой метод изменения цены"""
        self.priceMultiplier += 1

    def get_current_price(self):
        return self.basePrice * self.priceMultiplier

# --- ЖИТЕЛЬ (Субъект, который сам управляет связями) ---
class Villager:
    def __init__(self, game_instance, trades, hp: int = 20):
        self.game = game_instance 
        self.trades = trades
        self.emeralds = 0
        self.hpBar = hp
        self._is_hit = False

    def offerEmeralds(self, count: int):
        self.emeralds = count
        self.game.sync_visuals(self) # Прямое уведомление игры

    def takeEmeralds(self, count: int) -> bool:
        if self.emeralds >= count:
            self.emeralds -= count
            self.game.sync_visuals(self)
            return True
        return False

    def hit(self):
        if self.hpBar > 0:
            self._is_hit = True
            self.hpBar -= 2
            
            # Напрямую меняем состояние товаров (вместо Observer.update)
            for slot in self.trades:
                slot.increase_price()
            
            self.game.sync_visuals(self)
            self._is_hit = False

    def isHit(self) -> bool: return self._is_hit

# --- ГРАФИЧЕСКИЙ ДВИЖОК ---
class Game:
    def __init__(self):
        pygame.init()
        self.WIDTH, self.HEIGHT = 700, 500
        self.screen = pygame.display.set_mode((self.WIDTH, self.HEIGHT))
        pygame.display.set_caption("Villager Shop - No Observer (Flat Folder)")
        self.clock = pygame.time.Clock()
        self.font = pygame.font.SysFont("Arial", 22, bold=True)
        self.game_over = False

        self._load_resources()

        self.center_pos = (self.WIDTH // 2, self.HEIGHT // 2 + 40)
        self.current_mood = self.mood_icons["Neutral"]

        # Список товаров
        self.trades = [
            TradeSlot("Хлеб", 1),
            TradeSlot("Каменный меч", 4),
            TradeSlot("Железный нагрудник", 12),
            TradeSlot("Алмазная кирка", 20)
        ]
        
        # Создаем жителя и передаем ему ссылки на зависимости
        self.villager = Villager(self, self.trades)
        
        self.active_index = 0
        self.show_item = False 

    def _load_resources(self):
        """Загрузка ресурсов из текущей директории"""
        def load_img(name, w, h):
            path = os.path.join(BASE_PATH, name)
            try:
                img = pygame.image.load(path).convert_alpha()
                return pygame.transform.scale(img, (w, h))
            except:
                print(f"Ошибка: не найден файл {name}")
                surf = pygame.Surface((w, h))
                surf.fill((200, 0, 200)) # Пурпурный - классический цвет ошибки текстур
                return surf
        
        self.img_bg = load_img("background.jpg", self.WIDTH, self.HEIGHT)
        self.img_death_bg = load_img("dead.png", self.WIDTH, self.HEIGHT)
        self.img_villager = load_img("villager_base.png", 140, 240)
        self.img_golem = load_img("golem.png", 200, 300)
        
        self.item_images = {
            "Хлеб": load_img("bread.png", 140, 240),
            "Каменный меч": load_img("sword.png", 140, 240),
            "Железный нагрудник": load_img("chest.png", 140, 240),
            "Алмазная кирка": load_img("pixaxe.png", 140, 240)
        }
        self.mood_icons = {
            "Neutral": load_img("mood_neutral.png", 80, 80),
            "Angry": load_img("mood_angry.png", 80, 80),
            "Interested": load_img("mood_interested.png", 80, 80)
        }

    def sync_visuals(self, villager: Villager):
        """Ручная синхронизация состояния визуала"""
        if villager.hpBar <= 0:
            self.game_over = True
        elif villager.isHit():
            self.current_mood = self.mood_icons["Angry"]
        elif villager.emeralds > 0:
            self.current_mood = self.mood_icons["Interested"]
        else:
            self.current_mood = self.mood_icons["Neutral"]

    def handle_input(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT: 
                pygame.quit()
                sys.exit()
                
            if self.game_over:
                if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                    pygame.quit(); sys.exit()
                continue
            
            if event.type == pygame.MOUSEBUTTONDOWN:
                rect = self.img_villager.get_rect(center=self.center_pos)
                if rect.collidepoint(event.pos):
                    if event.button == 1: # Удар
                        self.villager.hit()
                    elif event.button == 3: # Добавить изумруд
                        self.villager.offerEmeralds(self.villager.emeralds + 1)

            if event.type == pygame.KEYDOWN:
                if event.key in [pygame.K_1, pygame.K_2, pygame.K_3, pygame.K_4]:
                    self.active_index = event.key - pygame.K_1
                    self.show_item = True
                
                if event.key == pygame.K_ESCAPE or event.key == pygame.K_TAB:
                    self.show_item = False
                
                if event.key == pygame.K_SPACE:
                    active_trade = self.trades[self.active_index]
                    if self.villager.takeEmeralds(active_trade.get_current_price()):
                        print(f"Куплено: {active_trade.itemName}")

    def draw(self):
        if not self.game_over:
            # Слой 0: Фон
            self.screen.blit(self.img_bg, (0, 0))
            
            active_trade = self.trades[self.active_index]
            
            # Слой 1: Центральный объект
            if self.show_item:
                current_img = self.item_images[active_trade.itemName]
            else:
                current_img = self.img_villager
                
            img_rect = current_img.get_rect(center=self.center_pos)
            self.screen.blit(current_img, img_rect)

            # Слой 2: Настроение
            mood_rect = self.current_mood.get_rect(center=(self.center_pos[0], img_rect.top - 45))
            self.screen.blit(self.current_mood, mood_rect)
            
            # Слой 3: Текстовый интерфейс
            self.screen.blit(self.font.render(f"HP: {self.villager.hpBar}", True, (200,0,0)), (20,20))
            self.screen.blit(self.font.render(f"Изумруды: {self.villager.emeralds}", True, (255,255,255)), (20,55))
            
            # Подпись выбранного товара и цены
            status = f"ВЫБРАНО: {active_trade.itemName} | Цена: {active_trade.get_current_price()}"
            info_text = self.font.render(status, True, (255, 255, 0))
            self.screen.blit(info_text, (self.WIDTH - info_text.get_width() - 20, 20))
            
            # Подсказка
            hint = "ЛКМ: Удар | ПКМ: Монета | 1-4: Выбор | ПРОБЕЛ: Купить"
            hint_surf = self.font.render(hint, True, (255,255,255))
            self.screen.blit(hint_surf, (self.WIDTH//2 - hint_surf.get_width()//2, self.HEIGHT-40))
        else:
            # ЭКРАН СМЕРТИ
            self.screen.blit(self.img_death_bg, (0, 0))
            golem_rect = self.img_golem.get_rect(center=(self.WIDTH * 3 // 4, self.HEIGHT//2))
            self.screen.blit(self.img_golem, golem_rect)
            
            death_msg = self.font.render("пупупу", True, (255, 255, 255))
            self.screen.blit(death_msg, (self.WIDTH//2 - death_msg.get_width()//2, 50))
            
            hint_exit = self.font.render("Нажми Esc для выхода", True, (255, 255, 255))
            self.screen.blit(hint_exit, (self.WIDTH//2 - hint_exit.get_width()//2, self.HEIGHT - 40))

        pygame.display.flip()

    def run(self):
        while True:
            self.handle_input()
            self.draw()
            self.clock.tick(60)

if __name__ == "__main__":
    Game().run()