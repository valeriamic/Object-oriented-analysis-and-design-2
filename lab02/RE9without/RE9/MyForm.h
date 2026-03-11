#pragma once

#include <stdlib.h> 
#include <ctime>    

namespace RE9 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;
	using namespace System::IO;

	// Сущность Крови (теперь используется напрямую)
	public ref class Blood {
	private: int h, al, an;
	public:
		Blood(int _h, int _al, int _an) : h(_h), al(_al), an(_an) {}
		int ExtractIron() { return h; }
		int GetAlcohol() { return al; }
		int GetAntibodies() { return an; }
	};

	// Станции теперь работают напрямую с классом Blood без посредников
	public ref class AmmoPress {
	public:
		bool Craft(Blood^ b, int count) { return b->ExtractIron() >= (count * 5); }
	};

	public ref class MedLab {
	public:
		bool Produce(Blood^ b, int count) { return b->GetAlcohol() >= (count * 30); }
	};

	public ref class ChemStation {
	public:
		bool Create(Blood^ b, int count) { return b->GetAntibodies() >= (count * 60); }
	};

	public ref class MyForm : public System::Windows::Forms::Form
	{
	private:
		int currentBlood, health = 150, killCount = 0;
		int inventoryAmmo = 10, inventoryHeal = 0, inventoryExplo = 2;

		Blood^ bloodSource;
		AmmoPress^ ammoPress;
		MedLab^ medLab;
		ChemStation^ chemStation;

		System::Windows::Forms::PictureBox^ picAmmo, ^ picHeal, ^ picExplo, ^ picFinal;
		System::Windows::Forms::NumericUpDown^ numAmmo, ^ numHeal, ^ numExplo;
		System::Windows::Forms::Button^ btnAmmo, ^ btnHeal, ^ btnExplo, ^ btnKillZombie, ^ btnUseHeal, ^ btnDestroy, ^ btnSave;
		System::Windows::Forms::Label^ lblBlood, ^ lblCostInfo, ^ lblInventory, ^ lblHealth, ^ lblFinalText;
		System::Windows::Forms::ProgressBar^ prgHealth;

	public:
		MyForm(void) {
			srand((unsigned int)time(0));
			currentBlood = rand() % 101 + 150;
			InitializeComponent();
			InitSystems();
			LoadImages();
			UpdateDisplay();
		}

	private:
		void InitSystems() {
			this->bloodSource = gcnew Blood(currentBlood, currentBlood, currentBlood);
			this->ammoPress = gcnew AmmoPress();
			this->medLab = gcnew MedLab();
			this->chemStation = gcnew ChemStation();
		}

		void UpdateDisplay() {
			// Проверка на гибель
			if (health <= 0) {
				health = 0;
				if (this->prgHealth) this->prgHealth->Value = 0;
				MessageBox::Show(L"Грейс погибла от ран... Миссия провалена.", L"Game Over", MessageBoxButtons::OK, MessageBoxIcon::Stop);
				Application::Exit();
				return;
			}

			this->lblBlood->Text = L"Кровь Грейс: " + currentBlood.ToString() + L" мл | Убито: " + killCount + L"/20";
			this->lblInventory->Text = L"Инвентарь:\nПатроны: " + inventoryAmmo + L" | Гранаты: " + inventoryExplo + L"\nИнъектор: " + inventoryHeal;

			int totalCost = ((int)numAmmo->Value * 5) + ((int)numHeal->Value * 30) + ((int)numExplo->Value * 60);
			this->lblCostInfo->Text = L"Стоимость крафта: " + totalCost.ToString() + L" мл";

			int safeHealth = (health > 150) ? 150 : health;
			this->prgHealth->Value = safeHealth;
			this->lblHealth->Text = L"HP: " + health + L" / 150";

			if (killCount >= 20) this->ShowFinalChoice();
		}

		void ShowFinalChoice() {
			for each (Control ^ c in this->Controls) c->Visible = false;

			String^ exePath = Path::GetDirectoryName(Application::ExecutablePath) + L"\\";
			String^ choiceImg = L"choice.jpg";

			if (File::Exists(exePath + choiceImg)) {
				FileStream^ fs = gcnew FileStream(exePath + choiceImg, FileMode::Open, FileAccess::Read);
				this->picFinal->Image = Image::FromStream(fs);
				fs->Close();
			}

			this->lblFinalText->Visible = true;
			this->picFinal->Visible = true;
			this->btnDestroy->Visible = true;
			this->btnSave->Visible = true;
			this->lblFinalText->Text = L"Вы дошли до конца. Судьба Элписа в ваших руках.";
		}

		void LoadImages() {
			String^ exePath = Path::GetDirectoryName(Application::ExecutablePath) + L"\\";
			array<String^>^ files = { L"ammo.png", L"heal.png", L"explo.png" };
			array<PictureBox^>^ boxes = { picAmmo, picHeal, picExplo };

			for (int i = 0; i < 3; i++) {
				String^ fullPath = exePath + files[i];
				if (!File::Exists(fullPath)) fullPath = exePath + L"..\\" + files[i];
				if (File::Exists(fullPath)) {
					FileStream^ fs = gcnew FileStream(fullPath, FileMode::Open, FileAccess::Read);
					boxes[i]->Image = Image::FromStream(fs);
					fs->Close();
				}
			}
		}

		// Обработчики событий с прямой передачей bloodSource
		void OnAmmoClick(Object^ sender, EventArgs^ e) {
			int c = (int)numAmmo->Value;
			if (c > 0 && ammoPress->Craft(bloodSource, c)) {
				currentBlood -= (c * 5); inventoryAmmo += c; numAmmo->Value = 0;
				bloodSource = gcnew Blood(currentBlood, currentBlood, currentBlood);
				UpdateDisplay();
			}
		}

		void OnHealClick(Object^ sender, EventArgs^ e) {
			int c = (int)numHeal->Value;
			if (c > 0 && medLab->Produce(bloodSource, c)) {
				currentBlood -= (c * 30); inventoryHeal += c; numHeal->Value = 0;
				bloodSource = gcnew Blood(currentBlood, currentBlood, currentBlood);
				UpdateDisplay();
			}
		}

		void OnExploClick(Object^ sender, EventArgs^ e) {
			int c = (int)numExplo->Value;
			if (c > 0 && chemStation->Create(bloodSource, c)) {
				currentBlood -= (c * 60); inventoryExplo += c; numExplo->Value = 0;
				bloodSource = gcnew Blood(currentBlood, currentBlood, currentBlood);
				UpdateDisplay();
			}
		}

		void OnKillZombieClick(Object^ sender, EventArgs^ e) {
			if (inventoryAmmo >= 2) { inventoryAmmo -= 2; health -= 15; killCount++; currentBlood += 40; }
			else if (inventoryExplo >= 1) { inventoryExplo -= 1; killCount++; currentBlood += 40; }
			else { health -= 30; MessageBox::Show(L"Нет боеприпасов! Зомби ранил вас!"); }

			bloodSource = gcnew Blood(currentBlood, currentBlood, currentBlood);
			UpdateDisplay();
		}

		void OnNumericChanged(Object^ sender, EventArgs^ e) { UpdateDisplay(); }

		void OnUseHealClick(Object^ sender, EventArgs^ e) {
			if (inventoryHeal > 0 && health < 150) {
				inventoryHeal--; health += 50;
				if (health > 150) health = 150;
				UpdateDisplay();
			}
		}

		void OnFinalChoice(Object^ sender, EventArgs^ e) {
			Button^ b = (Button^)sender;
			String^ exePath = Path::GetDirectoryName(Application::ExecutablePath) + L"\\";
			String^ imgFile = (b->Text == L"Уничтожить Элпис") ? L"destroy.jpeg" : L"save.png";

			lblFinalText->Text = (b->Text == L"Уничтожить Элпис") ?
				L"Элпис уничтожен. Леон погиб. Лекарство от Т-Вируса утеряно" :
				L"Элпис освобождён. Леон выжил. Мир получил лекарство от Т-Вируса";

			if (File::Exists(exePath + imgFile)) {
				FileStream^ fs = gcnew FileStream(exePath + imgFile, FileMode::Open, FileAccess::Read);
				this->picFinal->Image = Image::FromStream(fs);
				fs->Close();
				this->picFinal->Visible = true;
			}

			btnDestroy->Visible = false;
			btnSave->Visible = false;
			this->BackColor = System::Drawing::Color::Black;
			lblFinalText->ForeColor = System::Drawing::Color::White;
		}

		void InitializeComponent(void);
	};

	// (Код InitializeComponent остается идентичным, так как визуальная часть не меняется)
	inline void MyForm::InitializeComponent(void) {
		this->lblBlood = gcnew Label();
		this->btnKillZombie = gcnew Button();
		this->picAmmo = gcnew PictureBox();
		this->picHeal = gcnew PictureBox();
		this->picExplo = gcnew PictureBox();
		this->numAmmo = gcnew NumericUpDown();
		this->numHeal = gcnew NumericUpDown();
		this->numExplo = gcnew NumericUpDown();
		this->btnAmmo = gcnew Button();
		this->btnHeal = gcnew Button();
		this->btnExplo = gcnew Button();
		this->btnUseHeal = gcnew Button();
		this->lblHealth = gcnew Label();
		this->prgHealth = gcnew ProgressBar();
		this->lblInventory = gcnew Label();
		this->lblCostInfo = gcnew Label();
		this->lblFinalText = gcnew Label();
		this->btnDestroy = gcnew Button();
		this->btnSave = gcnew Button();
		this->picFinal = gcnew PictureBox();

		this->SuspendLayout();
		this->ClientSize = System::Drawing::Size(800, 600);
		this->Text = L"Umbrella Lab";

		this->numAmmo->ValueChanged += gcnew EventHandler(this, &MyForm::OnNumericChanged);
		this->numHeal->ValueChanged += gcnew EventHandler(this, &MyForm::OnNumericChanged);
		this->numExplo->ValueChanged += gcnew EventHandler(this, &MyForm::OnNumericChanged);

		this->lblBlood->Location = System::Drawing::Point(30, 20);
		this->lblBlood->AutoSize = true;
		this->lblBlood->Font = gcnew System::Drawing::Font(L"Impact", 16);

		this->btnKillZombie->Location = System::Drawing::Point(580, 15);
		this->btnKillZombie->Size = System::Drawing::Size(180, 50);
		this->btnKillZombie->Text = L"Убить зомби";
		this->btnKillZombie->Click += gcnew EventHandler(this, &MyForm::OnKillZombieClick);

		this->picAmmo->Location = System::Drawing::Point(50, 100); this->picAmmo->Size = System::Drawing::Size(200, 200);
		this->picAmmo->SizeMode = PictureBoxSizeMode::Zoom;
		this->picHeal->Location = System::Drawing::Point(300, 100); this->picHeal->Size = System::Drawing::Size(200, 200);
		this->picHeal->SizeMode = PictureBoxSizeMode::Zoom;
		this->picExplo->Location = System::Drawing::Point(550, 100); this->picExplo->Size = System::Drawing::Size(200, 200);
		this->picExplo->SizeMode = PictureBoxSizeMode::Zoom;

		this->numAmmo->Location = System::Drawing::Point(50, 310); this->numAmmo->Size = System::Drawing::Size(200, 25);
		this->numHeal->Location = System::Drawing::Point(300, 310); this->numHeal->Size = System::Drawing::Size(200, 25);
		this->numExplo->Location = System::Drawing::Point(550, 310); this->numExplo->Size = System::Drawing::Size(200, 25);

		this->btnAmmo->Location = System::Drawing::Point(50, 340); this->btnAmmo->Size = System::Drawing::Size(200, 40);
		this->btnAmmo->Text = L"Синтез патронов";
		this->btnAmmo->Click += gcnew EventHandler(this, &MyForm::OnAmmoClick);

		this->btnHeal->Location = System::Drawing::Point(300, 340); this->btnHeal->Size = System::Drawing::Size(200, 40);
		this->btnHeal->Text = L"Синтез инъектора";
		this->btnHeal->Click += gcnew EventHandler(this, &MyForm::OnHealClick);

		this->btnExplo->Location = System::Drawing::Point(550, 340); this->btnExplo->Size = System::Drawing::Size(200, 40);
		this->btnExplo->Text = L"Синтез гранаты";
		this->btnExplo->Click += gcnew EventHandler(this, &MyForm::OnExploClick);

		this->lblCostInfo->Location = System::Drawing::Point(50, 400); this->lblCostInfo->AutoSize = true;

		this->btnUseHeal->Location = System::Drawing::Point(300, 400); this->btnUseHeal->Size = System::Drawing::Size(200, 120);
		this->btnUseHeal->Text = L"Использовать инъектор";
		this->btnUseHeal->BackColor = System::Drawing::Color::DarkGreen;
		this->btnUseHeal->ForeColor = System::Drawing::Color::White;
		this->btnUseHeal->Click += gcnew EventHandler(this, &MyForm::OnUseHealClick);

		this->prgHealth->Location = System::Drawing::Point(550, 430); this->prgHealth->Size = System::Drawing::Size(200, 25);
		this->prgHealth->Maximum = 150;
		this->lblHealth->Location = System::Drawing::Point(550, 400); this->lblHealth->AutoSize = true;

		this->lblInventory->Location = System::Drawing::Point(550, 470); this->lblInventory->Size = System::Drawing::Size(200, 100);
		this->lblInventory->BackColor = System::Drawing::Color::Black;
		this->lblInventory->ForeColor = System::Drawing::Color::Lime;

		this->lblFinalText->Location = System::Drawing::Point(50, 50);
		this->lblFinalText->Size = System::Drawing::Size(700, 60);
		this->lblFinalText->Visible = false;
		this->lblFinalText->TextAlign = ContentAlignment::MiddleCenter;
		this->lblFinalText->Font = gcnew System::Drawing::Font(L"Impact", 20);

		this->picFinal->Location = System::Drawing::Point(200, 120);
		this->picFinal->Size = System::Drawing::Size(400, 300);
		this->picFinal->SizeMode = PictureBoxSizeMode::Zoom;
		this->picFinal->Visible = false;

		this->btnDestroy->Location = System::Drawing::Point(150, 450); this->btnDestroy->Size = System::Drawing::Size(200, 60);
		this->btnDestroy->Text = L"Уничтожить Элпис"; this->btnDestroy->Visible = false;
		this->btnDestroy->Click += gcnew EventHandler(this, &MyForm::OnFinalChoice);

		this->btnSave->Location = System::Drawing::Point(450, 450); this->btnSave->Size = System::Drawing::Size(200, 60);
		this->btnSave->Text = L"Освободить Элпис"; this->btnSave->Visible = false;
		this->btnSave->Click += gcnew EventHandler(this, &MyForm::OnFinalChoice);

		this->Controls->Add(this->lblBlood);
		this->Controls->Add(this->btnKillZombie);
		this->Controls->Add(this->picAmmo); this->Controls->Add(this->picHeal); this->Controls->Add(this->picExplo);
		this->Controls->Add(this->numAmmo); this->Controls->Add(this->numHeal); this->Controls->Add(this->numExplo);
		this->Controls->Add(this->btnAmmo); this->Controls->Add(this->btnHeal); this->Controls->Add(this->btnExplo);
		this->Controls->Add(this->btnUseHeal); this->Controls->Add(this->lblCostInfo);
		this->Controls->Add(this->lblHealth); this->Controls->Add(this->prgHealth); this->Controls->Add(this->lblInventory);
		this->Controls->Add(this->lblFinalText);
		this->Controls->Add(this->picFinal);
		this->Controls->Add(this->btnDestroy);
		this->Controls->Add(this->btnSave);

		this->ResumeLayout(false);
		this->PerformLayout();
	}
}

[System::STAThreadAttribute]
int main() {
	System::Windows::Forms::Application::EnableVisualStyles();
	System::Windows::Forms::Application::Run(gcnew RE9::MyForm());
	return 0;
}