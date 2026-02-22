<h3 align="center"> Общая идея. </h3>
Проблема: в рамках проведения кубка большого поршня необходимо определить, какая машина будет представлять спонсора - моторное масло Ржавейка, и сформировать его гоночную конфигурацию, чтобы гонщик не опозорил такой успешный бренд.  

Каждый потенциальный участник обладает различными характеристиками: скорость, опыт гонок, тип двигателя, внешний вид, специальные способности. Кандидатами могут выступать: Молния Маккуин, Круз Рамирес, Док Хадсон, Джексон Шторм, Франческо Бернулли, Мэтр. Если создавать участников через конструктор с большим числом параметров, мы запутаемся и в конце-концов, если Ржавейка найдёт источник нефти, обогатится и решит нанять новых агентов, у нас возникнут трудности с расширением списка кандидатов.  

Цель: Разработать систему пошагового формирования объекта «Участник гонки», представляющего спонсора, с использованием паттерна строителя. Система должна позволять выбирать кандидата и формировать его гоночную конфигурацию.  

Описание: Маркетолог департамента моторного масла Ржавейка выбирает машинку, которая будет представлять бренд в гонке. После выбора выполняется последовательное построение конфигурации: назначение базовых характеристик, настройка двигателя, добавление спонсорского оформления, настройка гоночных параметров, подготовка к соревнованию. Каждый кандидат имеет собственные параметры сборки. Результатом является объект RaceParticipant, готовый к гоночке.  

Применение паттерна Builder: 
1. Director управляет порядком подготовки участника.  
2. ParticipantBuilder - абстрактный строитель. Методы: buildStats(), buildEngine(), buildSponsorDesign(), buildRaceSetup(), getResult().  
3. Конкретные строители (McQueenBuilder, CruzBuilder, HudsonBuilder, StormBuilder, BernulliBuilder, MaterBuilder) формируют участников по собственной логике.  
4. Продукт RaceParticipant содержит имя участника, настройку двигателя, мощность, цвет, оформление спонсора, опыт и метод готовности к гонке.  
<img width="851" height="981" alt="ТАЧКИ (1)" src="https://github.com/user-attachments/assets/9aec43ff-b3d4-4842-950e-404ae683e000" />

<h3 align="center"> Реализация идеи без строителя. </h3>

```cs
using System;

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

        public void SetDesign(bool isSponsor)
        {
            SponsorDesign = isSponsor;
        }

        public void CheckDesign()
        {
            if (!SponsorDesign)
            {
                Console.WriteLine($"[Система]: {Name} не имеет символики 'Ржавейка'. Исправляем...");
                SetDesign(true);
            }
        }

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
            SponsorDesign = false;
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
            SponsorDesign = false;
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
            SponsorDesign = false;
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
            SponsorDesign = false;
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
            SponsorDesign = false;
        }
    }

    class Program
    {
        static void Main()
        {
            RaceParticipant[] participants = {
                new McQueen(),
                new Mater(),
                new Cruz(),
                new Hudson(),
                new Storm(),
                new Bernulli()
            };

            foreach (var car in participants)
            {
                car.Configure();
                car.CheckDesign();
                car.ReadyToRace();
            }
        }
    }
}
```

<img width="471" height="641" alt="ТАЧКИбезСТРОИТЕЛЯ (1)" src="https://github.com/user-attachments/assets/1fc411e4-3fb9-411f-9c9d-ac7ca67068e5" />


<h3 align="center"> Реализация идеи со строителем. </h3>

```cs
using System;
using System.Collections.Generic;

namespace RustezeRacing
{
    public class RaceParticipant
    {
        public string Name { get; set; }
        public string Setting { get; set; }
        public int Power { get; set; }
        public string Color { get; set; }
        public bool SponsorDesign { get; set; }
        public int YearsExp { get; set; }

        public void ReadyToRace()
        {
            Console.WriteLine($"Участник: {Name}");
            Console.WriteLine($"- {Power} HP, {YearsExp} years exp");
            Console.WriteLine($"- {Color}, Sponsor: {SponsorDesign}");
            Console.WriteLine($"- Setup: {Setting}\n");
        }
    }

    public abstract class ParticipantBuilder
    {
        protected RaceParticipant car = new RaceParticipant();

        public abstract void BuildStats();
        public abstract void BuildEngine();
        public abstract void BuildDesign();
        public abstract void BuildSetup();

        public void SetSponsorDesign(bool isSponsor) => car.SponsorDesign = isSponsor;
        public RaceParticipant GetResult() => car;
    }

    public class McQueenBuilder : ParticipantBuilder
    {
        public override void BuildStats() { car.Name = "Молния Маккуин"; car.YearsExp = 15; }
        public override void BuildEngine() => car.Power = 750;
        public override void BuildDesign() { car.Color = "Красный"; car.SponsorDesign = true; }
        public override void BuildSetup() => car.Setting = "Универсал";
    }

    public class MaterBuilder : ParticipantBuilder
    {
        public override void BuildStats() { car.Name = "Мэтр"; car.YearsExp = 10; }
        public override void BuildEngine() => car.Power = 300;
        public override void BuildDesign() { car.Color = "Ржавый"; car.SponsorDesign = false; }
        public override void BuildSetup() => car.Setting = "Грунтовая дорога";
    }

    public class CruzBuilder : ParticipantBuilder
    {
        public override void BuildStats() { car.Name = "Круз Рамирес"; car.YearsExp = 3; }
        public override void BuildEngine() => car.Power = 900;
        public override void BuildDesign() { car.Color = "Жёлтый"; car.SponsorDesign = false; }
        public override void BuildSetup() => car.Setting = "Гоночный трек";
    }

    public class StormBuilder : ParticipantBuilder
    {
        public override void BuildStats() { car.Name = "Джексон Шторм"; car.YearsExp = 3; }
        public override void BuildEngine() => car.Power = 900;
        public override void BuildDesign() { car.Color = "Чёрный"; car.SponsorDesign = false; }
        public override void BuildSetup() => car.Setting = "Гоночный трек";
    }

    public class HudsonBuilder : ParticipantBuilder
    {
        public override void BuildStats() { car.Name = "Док Хадсон"; car.YearsExp = 40; }
        public override void BuildEngine() => car.Power = 450;
        public override void BuildDesign() { car.Color = "Тёмно синий"; car.SponsorDesign = false; }
        public override void BuildSetup() => car.Setting = "Универсал";
    }

    public class BernoulliBuilder : ParticipantBuilder
    {
        public override void BuildStats() { car.Name = "Франческо Бернулли"; car.YearsExp = 15; }
        public override void BuildEngine() => car.Power = 750;
        public override void BuildDesign() { car.Color = "Красно-зелёный"; car.SponsorDesign = false; }
        public override void BuildSetup() => car.Setting = "Гоночный трек";
    }

    public class Director
    {
        public void Construct(ParticipantBuilder builder)
        {
            builder.BuildStats();
            builder.BuildEngine();
            builder.BuildDesign();
            builder.BuildSetup();
            CheckDesign(builder);
        }

        private void CheckDesign(ParticipantBuilder builder)
        {
            var participant = builder.GetResult();
            if (!participant.SponsorDesign)
            {
                Console.WriteLine($"У гонщика {participant.Name} нет логотипа. Исправление...");
                builder.SetSponsorDesign(true);
            }
        }
    }

    class Program
    {
        static void Main()
        {
            Director director = new Director();
            var builders = new List<ParticipantBuilder>
            {
                new McQueenBuilder(),
                new MaterBuilder(),
                new CruzBuilder(),
                new StormBuilder(),
                new HudsonBuilder(),
                new BernoulliBuilder()
            };

            foreach (var builder in builders)
            {
                director.Construct(builder);
                builder.GetResult().ReadyToRace();
            }
        }
    }
}
```



