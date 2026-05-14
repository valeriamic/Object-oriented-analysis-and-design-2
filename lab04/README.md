<img width="1091" height="751" alt="stalker11" src="https://github.com/user-attachments/assets/76e786ac-fc3e-4f48-97a4-b8b9d65f126b" />


Основная идея
Суть работы заключается в реализации системы игровых достижений для КПК сталкера с использованием паттерна Data Mapper. Преследуется цель сделать так, чтобы игровая логикабыла полностью изолирована от механизма хранения данных.
Диаграмма разделяет систему на три независимые части:
1. Слой Сущностей: Класс Achieve и его наследники (CounterAchieve, BooleanAchieve). Они отвечают за «жизнь» достижения в памяти игры: хранят текущий прогресс и проверяют условия выполнения через метод updateProgress. Они ничего не знают о том, как и куда они сохраняются.
2. Слой Хранилища: Интерфейс IDataSource и класс Database. Это технический уровень, который умеет выполнять только простые операции: прочитать строку или записать её в файл.
3. Слой Преобразователя: Класс AchieveMapper. Это «мозг» системы и главный посредник. Он берет «сырые» данные из таблицы, понимает, к какому типу достижения они относятся, и превращает их в живые объекты.

Задумка
1.  Метод getById в маппере запрашивает данные у IDataSource. Получив строку, маппер через приватный метод mapToEntity анализирует её и создает нужный объект — либо счетчик (например, «Убито зомбированных»), либо флаг (например, «Посетил Припять»).
2.  Игра работает с полученным объектом, вызывая updateProgress, когда сталкер совершает действия.
3.  Когда нужно сохранить игру, вызывается метод save в маппере. Маппер «сплющивает» объект обратно в строку через mapToRow и отдает её базе для записи.


Код с паттерном.

```java
import java.util.*;
import java.io.*;
import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.util.List;

abstract class Achieve {
    protected int id;
    protected String title;
    protected String description;

    public Achieve(int id, String title, String description) {
        this.id = id;
        this.title = title;
        this.description = description;
    }

    public abstract void updateProgress(Object data);
    public abstract boolean isCompleted();

    public int getId() { return id; }
    public String getTitle() { return title; }
    public String getDescription() { return description; }

    public abstract String toCsvRow();
}

class CounterAchieve extends Achieve {
    private int currCount;
    private final int targetCount;

    public CounterAchieve(int id, String title, String desc, int curr, int target) {
        super(id, title, desc);
        this.currCount = curr;
        this.targetCount = target;
    }

    @Override
    public void updateProgress(Object data) {
        if (data instanceof Integer) {
            this.currCount += (Integer) data;
            if (this.currCount > targetCount) this.currCount = targetCount;
        }
    }

    @Override
    public boolean isCompleted() {
        return currCount >= targetCount;
    }

    @Override
    public String toCsvRow() {
        return String.format("%d;counter;%s;%s;%d;%d;0", id, title, description, currCount, targetCount);
    }

    @Override
    public String toString() {
        return currCount >= targetCount ? "ВЫПОЛНЕНО" : currCount + " / " + targetCount;
    }
}

class BooleanAchieve extends Achieve {
    private boolean isTriggered;

    public BooleanAchieve(int id, String title, String desc, boolean triggered) {
        super(id, title, desc);
        this.isTriggered = triggered;
    }

    @Override
    public void updateProgress(Object data) {
        if (data instanceof Boolean) {
            this.isTriggered = (Boolean) data;
        }
    }

    @Override
    public boolean isCompleted() {
        return isTriggered;
    }

    @Override
    public String toCsvRow() {
        return String.format("%d;boolean;%s;%s;0;0;%d", id, title, description, isTriggered ? 1 : 0);
    }

    @Override
    public String toString() {
        return isTriggered ? "ВЫПОЛНЕНО" : "НЕ ВЫПОЛНЕНО";
    }
}

interface IDataSource {
    List<String> readData();
    void writeData(List<String> data);
}

class CsvDatabase implements IDataSource {
    private String filePath;

    public CsvDatabase(String path) {
        this.filePath = path;
    }

    @Override
    public List<String> readData() {
        List<String> lines = new ArrayList<>();
        try (BufferedReader br = new BufferedReader(new FileReader(filePath))) {
            String line = br.readLine();
            while ((line = br.readLine()) != null) {
                if (!line.trim().isEmpty()) lines.add(line);
            }
        } catch (IOException e) {
            System.err.println("Ошибка чтения базы: " + e.getMessage());
        }
        return lines;
    }

    @Override
    public void writeData(List<String> data) {
        try (PrintWriter pw = new PrintWriter(new FileWriter(filePath))) {
            pw.println("id;type;title;description;current_val;target_val;is_triggered");
            for (String line : data) {
                pw.println(line);
            }
        } catch (IOException e) {
            System.err.println("Ошибка сохранения базы: " + e.getMessage());
        }
    }
}

class AchieveMapper {
    private IDataSource dataSource;

    public AchieveMapper(IDataSource ds) {
        this.dataSource = ds;
    }

    public List<Achieve> loadAll() {
        List<Achieve> list = new ArrayList<>();
        for (String row : dataSource.readData()) {
            list.add(mapToEntity(row));
        }
        return list;
    }

    public void saveAll(List<Achieve> achieves) {
        List<String> rows = new ArrayList<>();
        for (Achieve a : achieves) {
            rows.add(a.toCsvRow());
        }
        dataSource.writeData(rows);
    }

    private Achieve mapToEntity(String row) {
        String[] p = row.split(";");
        int id = Integer.parseInt(p[0]);
        String type = p[1];
        String title = p[2];
        String desc = p[3];
        int cur = Integer.parseInt(p[4]);
        int tar = Integer.parseInt(p[5]);
        boolean trig = p[6].equals("1");

        if (type.equalsIgnoreCase("counter")) {
            return new CounterAchieve(id, title, desc, cur, tar);
        } else {
            return new BooleanAchieve(id, title, desc, trig);
        }
    }
}

public class StalkerGame {
    public static void main(String[] args) {
        IDataSource db = new CsvDatabase("stalker_achievements.csv");
        AchieveMapper mapper = new AchieveMapper(db);

        List<Achieve> pda = mapper.loadAll();
        Scanner scanner = new Scanner(System.in);

        System.out.println("============== КПК: СИСТЕМА ДОСТИЖЕНИЙ ==============");

        while (true) {
            System.out.println("\n[МЕНЮ КПК]: 1-Список | 2-Действие | 3-Сохранить | 0-Выход");
            System.out.print(">> ");
            String cmd = scanner.next();

            if (cmd.equals("1")) {
                System.out.println("\n--- ВАШИ ДОСТИЖЕНИЯ ---");
                for (Achieve a : pda) System.out.println(a);
            }
            else if (cmd.equals("2")) {
                System.out.println("\nВыберите симуляцию события:");
                System.out.println("a) Продать КПК Сычу (+1 к 'Торговец информацией')");
                System.out.println("b) Раскрыть дело Тремора (Выполнить 'Сыщик')");
                System.out.println("c) Заработать 5000 рублей (+5000 к 'Состоятельный клиент')");
                System.out.print(">> ");
                String subCmd = scanner.next();

                if (subCmd.equals("a")) {
                    pda.get(13).updateProgress(1);
                    System.out.println("Данные переданы Сычу.");
                } else if (subCmd.equals("b")) {
                    pda.get(0).updateProgress(true);
                    System.out.println("Улики найдены. Преступник наказан.");
                } else if (subCmd.equals("c")) {
                    pda.get(14).updateProgress(5000);
                    System.out.println("Счёт пополнен.");
                }
            }
            else if (cmd.equals("3")) {
                mapper.saveAll(pda);
                System.out.println("Данные КПК синхронизированы с базой (CSV).");
            }
            else if (cmd.equals("0")) {
                break;
            }
        }
        System.out.println("Удачи в Зоне, сталкер.");
        scanner.close();
    }
}
```
