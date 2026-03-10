#pragma once

#include <stdlib.h> 
#include <ctime>    

namespace RE9 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Windows::Forms;
	using namespace System::Drawing;
	using namespace System::IO;

	// Предварительные объявления
	ref class Blood;
	ref class AmmoPress;
	ref class MedLab;
	ref class ChemStation;

	// --- КЛАСС ФОРМЫ ---
	public ref class MyForm : public System::Windows::Forms::Form
	{
	private:
		int currentBlood;
		Blood^ bloodSource;

		AmmoPress^ ammoPress;
		MedLab^ medLab;
		ChemStation^ chemStation;

		System::Windows::Forms::PictureBox^ picAmmo;
		System::Windows::Forms::PictureBox^ picHeal;
		System::Windows::Forms::PictureBox^ picExplo;
		System::Windows::Forms::NumericUpDown^ numAmmo;
		System::Windows::Forms::NumericUpDown^ numHeal;
		System::Windows::Forms::NumericUpDown^ numExplo;
		System::Windows::Forms::Button^ btnAmmo;
		System::Windows::Forms::Button^ btnHeal;
		System::Windows::Forms::Button^ btnExplo;
		System::Windows::Forms::Label^ lblBlood;
		System::Windows::Forms::Label^ lblCostInfo;

	public:
		MyForm(void)
		{
			srand((unsigned int)time(0));
			currentBlood = rand() % 101 + 150;
			InitializeComponent();
			InitSystems();
			LoadImages();
			UpdateDisplay();
		}

	private:
		void InitSystems();
		void UpdateDisplay();
		void OnAmmoClick(Object^ sender, EventArgs^ e);
		void OnHealClick(Object^ sender, EventArgs^ e);
		void OnExploClick(Object^ sender, EventArgs^ e);
		void OnUpdate(Object^ sender, EventArgs^ e) { UpdateDisplay(); }
		void InitializeComponent(void);
		void LoadImages();
	};

	// --- РЕСУРС (Теперь единственный и неповторимый) ---
	public ref class Blood {
	private:
		int hemoglobinLevel;
		int alcoholLevel;
		int antibodiesLevel;
	public:
		Blood(int h, int al, int an) : hemoglobinLevel(h), alcoholLevel(al), antibodiesLevel(an) {}
		int ExtractIron() { return hemoglobinLevel; }
		int GetAlcohol() { return alcoholLevel; }
		int GetAntibodies() { return antibodiesLevel; }
	};

	// --- СТАНЦИИ (Теперь жестко зависят от класса Blood) ---
	public ref class AmmoPress {
	public:
		bool Craft(Blood^ blood, int count) {
			// Напрямую вызываем специфический метод Blood
			return blood->ExtractIron() >= (count * 5);
		}
	};

	public ref class MedLab {
	public:
		bool Produce(Blood^ blood, int count) {
			return blood->GetAlcohol() >= (count * 30);
		}
	};

	public ref class ChemStation {
	public:
		bool Create(Blood^ blood, int count) {
			return blood->GetAntibodies() >= (count * 60);
		}
	};

	// --- РЕАЛИЗАЦИЯ МЕТОДОВ ФОРМЫ ---

	inline void MyForm::InitSystems() {
		this->bloodSource = gcnew Blood(currentBlood, currentBlood, currentBlood);
		this->ammoPress = gcnew AmmoPress();
		this->medLab = gcnew MedLab();
		this->chemStation = gcnew ChemStation();
	}

	inline void MyForm::UpdateDisplay() {
		this->lblBlood->Text = L"Кровь Грейс: " + currentBlood.ToString() + L" мл";
		int total = ((int)numAmmo->Value * 5) + ((int)numHeal->Value * 30) + ((int)numExplo->Value * 60);
		this->lblCostInfo->Text = L"Стоимость крафта: " + total.ToString() + L" мл";
	}

	inline void MyForm::OnAmmoClick(Object^ sender, EventArgs^ e) {
		int count = (int)numAmmo->Value;
		if (count <= 0) return;

		// ПАТТЕРНА НЕТ: Передаем bloodSource напрямую в AmmoPress
		if (ammoPress->Craft(bloodSource, count)) {
			currentBlood -= (count * 5);
			bloodSource = gcnew Blood(currentBlood, currentBlood, currentBlood);
			numAmmo->Value = 0;
			MessageBox::Show(L"Боеприпасы готовы!");
		}
		else MessageBox::Show(L"Недостаточно гемоглобина!");
		UpdateDisplay();
	}

	inline void MyForm::OnHealClick(Object^ sender, EventArgs^ e) {
		int count = (int)numHeal->Value;
		if (count <= 0) return;

		if (medLab->Produce(bloodSource, count)) {
			currentBlood -= (count * 30);
			bloodSource = gcnew Blood(currentBlood, currentBlood, currentBlood);
			numHeal->Value = 0;
			MessageBox::Show(L"Инъектор синтезирован!");
		}
		else MessageBox::Show(L"Недостаточно спирта!");
		UpdateDisplay();
	}

	inline void MyForm::OnExploClick(Object^ sender, EventArgs^ e) {
		int count = (int)numExplo->Value;
		if (count <= 0) return;

		if (chemStation->Create(bloodSource, count)) {
			currentBlood -= (count * 60);
			bloodSource = gcnew Blood(currentBlood, currentBlood, currentBlood);
			numExplo->Value = 0;
			MessageBox::Show(L"Граната создана!");
		}
		else MessageBox::Show(L"Недостаточно антител!");
		UpdateDisplay();
	}

	inline void MyForm::InitializeComponent(void) {
		// (Код InitializeComponent и LoadImages остается без изменений)
		this->numAmmo = (gcnew System::Windows::Forms::NumericUpDown());
		this->numHeal = (gcnew System::Windows::Forms::NumericUpDown());
		this->numExplo = (gcnew System::Windows::Forms::NumericUpDown());
		this->btnAmmo = (gcnew System::Windows::Forms::Button());
		this->btnHeal = (gcnew System::Windows::Forms::Button());
		this->btnExplo = (gcnew System::Windows::Forms::Button());
		this->lblBlood = (gcnew System::Windows::Forms::Label());
		this->lblCostInfo = (gcnew System::Windows::Forms::Label());
		this->picAmmo = (gcnew System::Windows::Forms::PictureBox());
		this->picHeal = (gcnew System::Windows::Forms::PictureBox());
		this->picExplo = (gcnew System::Windows::Forms::PictureBox());
		this->SuspendLayout();

		this->lblBlood->Font = (gcnew System::Drawing::Font(L"Arial", 12, System::Drawing::FontStyle::Bold));
		this->lblBlood->ForeColor = System::Drawing::Color::DarkRed;
		this->lblBlood->Location = System::Drawing::Point(110, 30);
		this->lblBlood->AutoSize = true;

		this->picAmmo->Location = System::Drawing::Point(50, 80);
		this->picAmmo->Size = System::Drawing::Size(180, 200);
		this->picAmmo->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;

		this->picHeal->Location = System::Drawing::Point(270, 80);
		this->picHeal->Size = System::Drawing::Size(180, 200);
		this->picHeal->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;

		this->picExplo->Location = System::Drawing::Point(490, 80);
		this->picExplo->Size = System::Drawing::Size(180, 200);
		this->picExplo->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;

		this->numAmmo->Location = System::Drawing::Point(115, 290);
		this->numHeal->Location = System::Drawing::Point(335, 290);
		this->numExplo->Location = System::Drawing::Point(555, 290);

		this->numAmmo->ValueChanged += gcnew System::EventHandler(this, &MyForm::OnUpdate);
		this->numHeal->ValueChanged += gcnew System::EventHandler(this, &MyForm::OnUpdate);
		this->numExplo->ValueChanged += gcnew System::EventHandler(this, &MyForm::OnUpdate);

		this->btnAmmo->Location = System::Drawing::Point(50, 320);
		this->btnAmmo->Size = System::Drawing::Size(180, 30);
		this->btnAmmo->Text = L"Крафт Патронов (5)";
		this->btnAmmo->Click += gcnew System::EventHandler(this, &MyForm::OnAmmoClick);

		this->btnHeal->Location = System::Drawing::Point(270, 320);
		this->btnHeal->Size = System::Drawing::Size(180, 30);
		this->btnHeal->Text = L"Крафт Инъектора (30)";
		this->btnHeal->Click += gcnew System::EventHandler(this, &MyForm::OnHealClick);

		this->btnExplo->Location = System::Drawing::Point(490, 320);
		this->btnExplo->Size = System::Drawing::Size(180, 30);
		this->btnExplo->Text = L"Крафт Гранаты (60)";
		this->btnExplo->Click += gcnew System::EventHandler(this, &MyForm::OnExploClick);

		this->lblCostInfo->Location = System::Drawing::Point(50, 410);
		this->lblCostInfo->AutoSize = true;

		this->ClientSize = System::Drawing::Size(720, 480);
		this->Controls->Add(this->picAmmo);
		this->Controls->Add(this->picHeal);
		this->Controls->Add(this->picExplo);
		this->Controls->Add(this->lblBlood);
		this->Controls->Add(this->lblCostInfo);
		this->Controls->Add(this->btnAmmo);
		this->Controls->Add(this->btnHeal);
		this->Controls->Add(this->btnExplo);
		this->Controls->Add(this->numAmmo);
		this->Controls->Add(this->numHeal);
		this->Controls->Add(this->numExplo);
		this->Text = L"Umbrella Lab (No Pattern)";
		this->ResumeLayout(false);
		this->PerformLayout();
	}

	inline void MyForm::LoadImages() {
		String^ p = L"x64\\Debug\\";
		array<String^>^ f = { L"ammo.png", L"heal.png", L"explo.png" };
		array<PictureBox^>^ b = { picAmmo, picHeal, picExplo };
		for (int i = 0; i < 3; i++) {
			if (File::Exists(p + f[i])) {
				FileStream^ s = gcnew FileStream(p + f[i], FileMode::Open, FileAccess::Read);
				b[i]->Image = Image::FromStream(s);
				s->Close();
			}
			else b[i]->BackColor = Color::Gray;
		}
	}
}

[System::STAThreadAttribute]
int main() {
	System::Windows::Forms::Application::EnableVisualStyles();
	System::Windows::Forms::Application::Run(gcnew RE9::MyForm());
	return 0;
}