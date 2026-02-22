<h3 align="center"> Общая идея. </h3>
Проблема: в рамках проведения кубка большого поршня необходимо определить, какая машина будет представлять спонсора - моторное масло Ржавейка, и сформировать его гоночную конфигурацию, чтобы гонщик не опозорил такой успешный бренд. 

Каждый потанцевальный участник обладает различными характеристиками: скорость, опыт гонок, тип двигателя, внешний вид, специальные способности. Кандидатами могут выступать: Молния Маккуин, Круз Рамирес, Док Хадсон, Джексон Шторм, Франческо Бернулли, Мэтр. Если создавать участников через конструктор с большим числом параметров, мы запутаемся и в конце-концов, если Ржавейка найдёт источник нефти, обогатится и решит нанять новых агентов, у нас возникнут трудности с расширением списка кандидатов.

Цель: Разработать систему пошагового формирования объекта «Участник гонки», представляющего спонсора, с использованием паттерна строителя. Система должна позволять выбирать кандидата и формировать его гоночную конфигурацию.  

Описание: Маркетолог департамента моторного масла Ржавейка выбирает машинку, которая будет представлять бренд в гонке. После выбора выполняется последовательное построение конфигурации: назначение базовых характеристик, настройка двигателя, добавление спонсорского оформления, настройка гоночных параметров, подготовка к соревнованию. Каждый кандидат имеет собственные параметры сборки. Результатом является объект RaceParticipant, готовый к гоночке.  

Применение паттерна Builder: 
1. Director управляет порядком подготовки участника.  
2. ParticipantBuilder - абстрактный строитель. Методы: buildStats(), buildEngine(), buildSponsorDesign(), buildRaceSetup(), getResult().  
3. Конкретные строители (McQueenBuilder, CruzBuilder, HudsonBuilder, StormBuilder, BernulliBuilder, MaterBuilder) формируют участников по собственной логике.  
4. Продукт RaceParticipant содержит имя участника, настройку двигателя, мощность, цвет, оформление спонсора, опыт и метод готовности к гонке.  
<img width="851" height="761" alt="ТАЧКИ" src="https://github.com/user-attachments/assets/49260fb9-383b-4f5d-8fe9-5b8063324511" />

<h3 align="center"> Реализация идеи без строителя. </h3>

```cs
namespace RustezeRacing
{
    public abstract class RaceParticipant
    {
        public string Name { get; set; }
        public string Setting { get; set; }
        public int Power { get; set; }
        public string Color { get; set; }
        public bool SponsorDesign { get; set; }
        public int YearsExp { get; set; }
   
        public RaceParticipant(string name, int yearsExp)
        {
            Name = name;
            YearsExp = yearsExp;
        }

        public abstract void Configure();

        public void ReadyToRace()
        {
            Console.WriteLine($"Гонщик: {Name}");
            Console.WriteLine($"Мощность: {Power} л.с.");
            Console.WriteLine($"Настройка: {Setting}");
            Console.WriteLine($"Бренд Ржавейка: {(SponsorDesign ? "Да" : "Нет")}");
            Console.WriteLine("Статус: К заезду готов.\n");
        }
    }

    public class McQueen : RaceParticipant
    {
        public McQueen() : base("Молния Маккуин", 15) { }

        public override void Configure()
        {
            Power = 750;
            Color = "Красный";
            Setting = "Универсал";
            SponsorDesign = true;
        }
    }

    public class Mater : RaceParticipant
    {
        public Mater() : base("Мэтр", 10) { }

        public override void Configure()
        {
            Power = 300;
            Color = "Ржавый";
            Setting = "Грунтовая дорога";
            SponsorDesign = true;
        }
    }

    public class Cruz : RaceParticipant
    {
        public Cruz() : base("Круз", 3) { }

        public override void Configure()
        {
            Power = 900;
            Color = "Жёлтый";
            Setting = "Гоночный трек";
            SponsorDesign = true;
        }
    }

    public class Hudson : RaceParticipant
    {
        public Hudson() : base("Хадсон", 40) { }

        public override void Configure()
        {
            Power = 450;
            Color = "Тёмно синий";
            Setting = "Универсал";
            SponsorDesign = true;
        }
    }

    public class Storm : RaceParticipant
    {
        public Storm() : base("Шторм", 3) { }

        public override void Configure()
        {
            Power = 900;
            Color = "Чёрный";
            Setting = "Гоночный трек";
            SponsorDesign = true;
        }
    }

    public class Bernulli : RaceParticipant
    {
        public Bernulli() : base("Бернулли", 15) { }

        public override void Configure()
        {
            Power = 750;
            Color = "Красно-зелёный";
            Setting = "Гоночный трек";
            SponsorDesign = true;
        }
    }

    class Program
    {
        static void Main()
        {
            RaceParticipant lightning = new McQueen();
            lightning.Configure();
            lightning.ReadyToRace();

            RaceParticipant mater = new Mater();
            towMater.Configure();
            towMater.ReadyToRace();

            RaceParticipant cruz = new Cruz();
            lightning.Configure();
            lightning.ReadyToRace();

            RaceParticipant doc = new Hudson();
            lightning.Configure();
            lightning.ReadyToRace();

            RaceParticipant storm = new Storm();
            lightning.Configure();
            lightning.ReadyToRace();

            RaceParticipant italiano = new Bernulli();
            lightning.Configure();
            lightning.ReadyToRace();
        }
    }
}
```

<img width="471" height="561" alt="ТАЧКИбезСТРОИТЕЛЯ" src="https://github.com/user-attachments/assets/7c55ebeb-6f9e-470b-a06b-ddb1a15dcfd3" />




