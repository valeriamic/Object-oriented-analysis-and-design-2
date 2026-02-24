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

            picRacer.SizeMode = PictureBoxSizeMode.Zoom;

            string startPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "images/start.png");

            if (File.Exists(startPath))
            {
                try
                {
                    using (var stream = new FileStream(startPath, FileMode.Open, FileAccess.Read))
                    {
                        picRacer.Image = Image.FromStream(stream);
                    }
                    picRacer.Refresh();
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Ошибка при чтении файла: " + ex.Message);
                }
            }
            else
            {
                MessageBox.Show("Заставка не найдена! Проверьте папку bin/Debug/images/start.png");
            }
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