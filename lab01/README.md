<h3 align="center"> Общая идея. </h3>
Проблема: в рамках проведения кубка большого поршня необходимо определить, какая машина будет представлять спонсора - моторное масло Ржавейка, и сформировать его гоночную конфигурацию, чтобы гонщик не опозорил такой успешный бренд.  

Каждый потенциальный участник обладает различными характеристиками: имя, мощность, опыт гонок, настройка двигателя, внешний вид. Кандидатами могут выступать: Молния Маккуин, Круз Рамирес, Док Хадсон, Джексон Шторм, Франческо Бернулли, Мэтр. Если создавать участников через конструктор с большим числом параметров, мы запутаемся и в конце-концов, если Ржавейка найдёт источник нефти, обогатится и решит нанять новых агентов, у нас возникнут трудности с расширением списка кандидатов.  

Цель: Разработать систему пошагового формирования объекта «Участник гонки», представляющего спонсора, с использованием паттерна строителя. Система должна позволять выбирать кандидата и формировать его гоночную конфигурацию.  

Описание: Маркетолог департамента моторного масла Ржавейка выбирает машинку, которая будет представлять бренд в гонке. После выбора выполняется последовательное построение конфигурации: назначение базовых характеристик, настройка двигателя, добавление спонсорского оформления, настройка гоночных параметров, подготовка к соревнованию. Каждый кандидат имеет собственные параметры сборки. Результатом является объект RaceParticipant, готовый к гоночке.  

Применение паттерна Builder: 
1. Director управляет порядком подготовки участника.  
2. ParticipantBuilder - абстрактный строитель. Методы: buildStats(), buildEngine(), buildSponsorDesign(), buildRaceSetup(), getResult().  
3. Конкретные строители (McQueenBuilder, CruzBuilder, HudsonBuilder, StormBuilder, BernulliBuilder, MaterBuilder) формируют участников по собственной логике.  
4. Продукт RaceParticipant содержит имя участника, настройку двигателя, мощность, цвет, оформление спонсора, опыт и метод готовности к гонке.  
<img width="851" height="1021" alt="cars1" src="https://github.com/user-attachments/assets/f7f57ea3-5c80-4298-b695-901481a9d74d" />



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
<img width="471" height="641" alt="cars2" src="https://github.com/user-attachments/assets/4bd3f5f7-9445-40e4-a506-904d827e3e2b" />




<h3 align="center"> Реализация идеи со строителем. </h3>

```cs
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using System.IO;

namespace CarsApp
{
    public partial class Form1 : Form
    {
        Director director = new Director();
        private string lastUpgradedRacer = "";

        public Form1()
        {
            InitializeComponent();
        }

        private void UpdateDisplay(RaceParticipant racer)
        {
            lblStats.Text = $"Гонщик: {racer.Name}\n" +
                    $"Опыт (в годах): {racer.Exp}\n" +
                    $"Мощность: {racer.Power} л.с.\n" +
                    $"Настройки: {racer.Setup}\n" +
                    $"Дизайн: {(racer.SponsorDesign ? "С логотипом" : "Базовый")}";

            if (File.Exists(racer.ImagePath))
            {
                using (var stream = new FileStream(racer.ImagePath, FileMode.Open, FileAccess.Read))
                {
                    picRacer.Image = Image.FromStream(stream);
                }
            }
            else
            {
                MessageBox.Show("Файл не найден: " + racer.ImagePath);
            }
        }

        private ParticipantBuilder GetSelectedBuilder()
        {
            if (comboBoxRacers.SelectedItem == null) return null;
            string selection = comboBoxRacers.SelectedItem.ToString();

            switch (selection)
            {
                case "Молния Маккуин": return new McQueenBuilder();
                case "Мэтр": return new MaterBuilder();
                case "Круз Рамирес": return new CruzBuilder();
                case "Джексон Шторм": return new StormBuilder();
                case "Док Хадсон": return new HudsonBuilder();
                case "Франческо Бернулли": return new BernoulliBuilder();
                default: return null;
            }
        }

        private void comboBoxRacers_SelectedIndexChanged(object sender, EventArgs e)
        {
            var builder = GetSelectedBuilder();
            if (builder != null)
            {
                builder.BuildStats();
                builder.BuildEngine();
                builder.BuildSetup();
                builder.BuildExp();
                builder.BuildDesign();
                UpdateDisplay(builder.GetResult());
            }
        }

        private void btnBuild_Click(object sender, EventArgs e)
        {
            var builder = GetSelectedBuilder();
            if (builder == null) return;

            string currentSelection = comboBoxRacers.SelectedItem.ToString();

            if (lastUpgradedRacer == currentSelection)
            {
                MessageBox.Show("Будет достаточно и одного логотипа, никто не любит слишком навязчивую рекламу");
                return;
            }

            builder.BuildStats();
            builder.BuildDesign();
            builder.BuildEngine();
            builder.BuildExp();
            builder.BuildSetup();
            if (builder.GetResult().Name == "Молния Маккуин")
            {
                MessageBox.Show("Будет достаточно и одного логотипа, никто не любит слишком навязчивую рекламу");
                lastUpgradedRacer = currentSelection;
                UpdateDisplay(builder.GetResult());
                return;
            }

            director.Construct(builder);
            lastUpgradedRacer = currentSelection;
            UpdateDisplay(builder.GetResult());
        }
    }

    public class RaceParticipant
    {
        public string Name { get; set; }
        public int Power { get; set; }
        public string Setup { get; set; }
        public bool SponsorDesign { get; set; }
        public int Exp { get; set; }
        public string ImagePath { get; set; }
    }

    public abstract class ParticipantBuilder
    {
        protected RaceParticipant car = new RaceParticipant();
        public abstract void BuildStats();
        public abstract void BuildEngine();
        public abstract void BuildDesign();
        public abstract void BuildExp();
        public abstract void BuildSetup();

        public void SetSponsorDesign(bool isSponsor, string newImagePath)
        {
            car.SponsorDesign = isSponsor;
            car.ImagePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, newImagePath);
        }

        public RaceParticipant GetResult() => car;
    }

    public class McQueenBuilder : ParticipantBuilder
    {
        public override void BuildStats() => car.Name = "Молния Маккуин";
        public override void BuildExp() => car.Exp = 15;
        public override void BuildEngine() => car.Power = 750;
        public override void BuildSetup() => car.Setup = "Универсал";
        public override void BuildDesign() => SetSponsorDesign(true, "images/mcqueen1.png");
    }

    public class MaterBuilder : ParticipantBuilder
    {
        public override void BuildStats() => car.Name = "Мэтр";
        public override void BuildExp() => car.Exp = 10;
        public override void BuildEngine() => car.Power = 300;
        public override void BuildSetup() => car.Setup = "Грунтовая дорога";
        public override void BuildDesign() => SetSponsorDesign(false, "images/mater1.png");
    }

    public class CruzBuilder : ParticipantBuilder
    {
        public override void BuildStats() => car.Name = "Круз Рамирес";
        public override void BuildExp() => car.Exp = 3;
        public override void BuildEngine() => car.Power = 900;
        public override void BuildSetup() => car.Setup = "Гоночный трек";
        public override void BuildDesign() => SetSponsorDesign(false, "images/cruz1.png");
    }

    public class StormBuilder : ParticipantBuilder
    {
        public override void BuildStats() => car.Name = "Джексон Шторм";
        public override void BuildEngine() => car.Power = 900;
        public override void BuildExp() => car.Exp = 3;
        public override void BuildSetup() => car.Setup = "Гоночный трек";
        public override void BuildDesign() => SetSponsorDesign(false, "images/storm1.png");
    }

    public class HudsonBuilder : ParticipantBuilder
    {
        public override void BuildStats() => car.Name = "Док Хадсон";
        public override void BuildExp() => car.Exp = 40;
        public override void BuildEngine() => car.Power = 450;
        public override void BuildSetup() => car.Setup = "Универсал";
        public override void BuildDesign() => SetSponsorDesign(false, "images/doc1.png");
    }

    public class BernoulliBuilder : ParticipantBuilder
    {
        public override void BuildStats() => car.Name = "Франческо Бернулли";
        public override void BuildExp() => car.Exp = 15;
        public override void BuildEngine() => car.Power = 750;
        public override void BuildSetup() => car.Setup = "Гоночный трек";
        public override void BuildDesign() => SetSponsorDesign(false, "images/bernulli1.png");
    }

    public class Director
    {
        public void Construct(ParticipantBuilder builder)
        {
            builder.BuildStats();
            builder.BuildEngine();
            builder.BuildSetup();
            builder.BuildExp();
            builder.BuildDesign();

            if (!builder.GetResult().SponsorDesign)
            {
                MessageBox.Show($"У {builder.GetResult().Name} нет крутого логотипа. Исправляем это чучело...");

                string logoImg = "";
                var name = builder.GetResult().Name;
                if (name == "Мэтр") logoImg = "images/mater2.png";
                else if (name == "Круз Рамирес") logoImg = "images/cruz2.png";
                else if (name == "Джексон Шторм") logoImg = "images/storm2.png";
                else if (name == "Док Хадсон") logoImg = "images/doc2.png";
                else if (name == "Франческо Бернулли") logoImg = "images/bernulli2.png";

                builder.SetSponsorDesign(true, logoImg);
            }
        }
    }


}
```


<h3 align="center"> Выводы. </h3>  

В реализации с паттерном всем процессом управляет директор, т.е. работа строго структурирована по вызовам, директор - строитель - конкретный строитель, то есть код становится легко читаемым, для добавления нового персонажа надо создать нового строителя и он будет строить новую машинку.  

В реализации без паттерна объекты создают сами себя в методе Configure(), вручную в мэйне вызываются все методы, из-за чего любой из них можно просто забыть вызвать (проверка на дизайн, рисование нового дизайна).  
Для проекта моего масштаба паттерн избыточен, будет гораздо выгоднее из-за простоты кода начирикать код простыми конструкторами без использования строителя. Но если задача будет больше и будет масштабироваться, строитель будет полезнее просто для того, чтобы не заблудиться в трёх соснах аки конструкторах и условиях.  

В общем и целом, код с паттерном выглядит более профессионально, как будто я не студентка, а сижу в паддоке на трассе и выбираю, как собрать машинку для гонок.  


