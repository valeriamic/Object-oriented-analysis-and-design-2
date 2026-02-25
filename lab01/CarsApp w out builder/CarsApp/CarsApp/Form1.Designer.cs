namespace CarsApp
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            comboBoxRacers = new ComboBox();
            picRacer = new PictureBox();
            lblStats = new Label();
            btnBuild = new Button();
            ((System.ComponentModel.ISupportInitialize)picRacer).BeginInit();
            SuspendLayout();
            // 
            // comboBoxRacers
            // 
            comboBoxRacers.FormattingEnabled = true;
            comboBoxRacers.Items.AddRange(new object[] { "Молния Маккуин", "Круз Рамирес", "Док Хадсон", "Мэтр", "Джексон Шторм", "Франческо Бернулли" });
            comboBoxRacers.Location = new Point(685, 23);
            comboBoxRacers.Name = "comboBoxRacers";
            comboBoxRacers.Size = new Size(151, 28);
            comboBoxRacers.TabIndex = 0;
            comboBoxRacers.SelectedIndexChanged += comboBoxRacers_SelectedIndexChanged;
            // 
            // picRacer
            // 
            picRacer.Location = new Point(12, 11);
            picRacer.Name = "picRacer";
            picRacer.Size = new Size(663, 514);
            picRacer.SizeMode = PictureBoxSizeMode.Zoom;
            picRacer.TabIndex = 1;
            picRacer.TabStop = false;
            // 
            // lblStats
            // 
            lblStats.AutoSize = true;
            lblStats.Location = new Point(685, 91);
            lblStats.Name = "lblStats";
            lblStats.Size = new Size(120, 20);
            lblStats.TabIndex = 2;
            lblStats.Text = "Характеристики";
            // 
            // btnBuild
            // 
            btnBuild.Location = new Point(730, 467);
            btnBuild.Name = "btnBuild";
            btnBuild.Size = new Size(94, 29);
            btnBuild.TabIndex = 3;
            btnBuild.Text = "Ехала";
            btnBuild.UseVisualStyleBackColor = true;
            btnBuild.Click += btnBuild_Click;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(8F, 20F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(908, 537);
            Controls.Add(btnBuild);
            Controls.Add(lblStats);
            Controls.Add(picRacer);
            Controls.Add(comboBoxRacers);
            Name = "Form1";
            Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)picRacer).EndInit();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private ComboBox comboBoxRacers;
        private PictureBox picRacer;
        private Label lblStats;
        private Button btnBuild;
    }
}