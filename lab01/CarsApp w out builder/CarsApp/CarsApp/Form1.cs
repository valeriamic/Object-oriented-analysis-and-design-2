using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO;

namespace RustezeRacing
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            picRacer.SizeMode = PictureBoxSizeMode.Zoom;

            string startPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "images/start.png");
            if (File.Exists(startPath))
            {
                using (var stream = new FileStream(startPath, FileMode.Open, FileAccess.Read))
                {
                    picRacer.Image = Image.FromStream(stream);
                }
            }
        }

        private RaceParticipant GetSelectedRacer()
        {
            if (comboBoxRacers.SelectedItem == null) return null;
            string selection = comboBoxRacers.SelectedItem.ToString();

            switch (selection)
            {
                case "Молния Маккуин": return new McQueen();
                case "Мэтр": return new Mater();
                case "Круз Рамирес": return new Cruz();
                case "Джексон Шторм": return new Storm();
                case "Док Хадсон": return new Hudson();
                case "Франческо Бернулли": return new Bernulli();
                default: return null;
            }
        }

        private void UpdateDisplay(RaceParticipant racer)
        {
            lblStats.Text = $"Гонщик: {racer.Name}\n" +
                            $"Опыт: {racer.YearsExp} лет\n" +
                            $"Мощность: {racer.Power} л.с.\n" +
                            $"Настройка: {racer.Setting}\n" +
                            $"Дизайн: {(racer.SponsorDesign ? "С логотипом" : "Базовый")}";

            if (File.Exists(racer.ImagePath))
            {
                using (var stream = new FileStream(racer.ImagePath, FileMode.Open, FileAccess.Read))
                {
                    picRacer.Image = Image.FromStream(stream);
                }
            }
        }

        private void comboBoxRacers_SelectedIndexChanged(object sender, EventArgs e)
        {
            var racer = GetSelectedRacer();
            if (racer != null)
            {
                racer.Configure();
                UpdateDisplay(racer);
            }
        }

        private void btnBuild_Click(object sender, EventArgs e)
        {
            var racer = GetSelectedRacer();
            if (racer == null) return;

            racer.Configure();
            racer.CheckDesign();
            UpdateDisplay(racer);
        }
    }


    public abstract class RaceParticipant
    {
        public string Name { get; set; }
        public string Setting { get; set; }
        public int Power { get; set; }
        public bool SponsorDesign { get; set; }
        public int YearsExp { get; set; }
        public string ImagePath { get; set; }

        public RaceParticipant(string name, int yearsExp)
        {
            Name = name;
            YearsExp = yearsExp;
        }

        public abstract void Configure();

        protected void SetImagePath(string fileName)
        {
            ImagePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "images", fileName);
        }

        public void CheckDesign()
        {
            if (!SponsorDesign)
            {
                SponsorDesign = true;
                ImagePath = ImagePath.Replace("1.png", "2.png");
            }
        }
    }

    public class McQueen : RaceParticipant
    {
        public McQueen() : base("Молния Маккуин", 15) { }
        public override void Configure()
        {
            Power = 750; Setting = "Универсал"; SponsorDesign = true;
            SetImagePath("mcqueen1.png");
        }
    }
    public class Mater : RaceParticipant
    {
        public Mater() : base("Мэтр", 10) { }
        public override void Configure()
        {
            Power = 300; Setting = "Грунтовая дорога"; SponsorDesign = false;
            SetImagePath("mater1.png");
        }
    }

    public class Cruz : RaceParticipant
    {
        public Cruz() : base("Круз Рамирес", 3) { }
        public override void Configure()
        {
            Power = 900; Setting = "Гоночный трек"; SponsorDesign = false;
            SetImagePath("cruz1.png");
        }
    }

    public class Hudson : RaceParticipant
    {
        public Hudson() : base("Док Хадсон", 40) { }
        public override void Configure()
        {
            Power = 450; Setting = "Универсал"; SponsorDesign = false;
            SetImagePath("doc1.png");
        }
    }

    public class Storm : RaceParticipant
    {
        public Storm() : base("Джексон Шторм", 3) { }
        public override void Configure()
        {
            Power = 900; Setting = "Гоночный трек"; SponsorDesign = false;
            SetImagePath("storm1.png");
        }
    }

    public class Bernulli : RaceParticipant
    {
        public Bernulli() : base("Франческо Бернулли", 15) { }
        public override void Configure()
        {
            Power = 750; Setting = "Гоночный трек"; SponsorDesign = false;
            SetImagePath("bernulli1.png");
        }
    }
}