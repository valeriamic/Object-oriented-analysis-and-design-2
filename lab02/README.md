<h3 align="center"> Общая идея. </h3>
Проблема: Грейс, попадая в одну из лабораторий Амбрелла в Раккун-сити, понимает, что за ней идёт охота. Для выживания с помощью инъектора крови она собирает ресурс — Кровь (Blood). В лаборатории уже существуют готовые модули для крафта, но они были созданы для стандартных материалов: Станок для патронов (AmmoPress) ожидает на вход порох, медицинский стол (MedLab) ожидает антитела, химическая установка (ChemStation) для гранат и коктейлей Молотова ожидает горючую смесь. У класса Blood нет метода GetVolume(), у него есть специфические параметры: hemoglobinLevel, alcoholLevel и antibodiesLevel, и методы: Extraction(), GetAlcohol(), GetAntibodies(). Напрямую передать кровь в станок для патронов невозможно — интерфейсы несовместимы.  

Решение: Учёные из Амбрелла создают систему Био-адаптеров. Каждый адаптер выступает в роли преобразователя, который переводит биологические параметры крови в технические параметры, понятные уже созданным станкам. Они создают единый интерфейс IComponent с методом GetVolume(), который ожидают все станки. Затем для каждого типа крафта создаётся свой адаптер: BloodToAmmoAdapter: извлекает из крови железо и нужные ферменты для создания порохового заряда, BloodToHealAdapter: извлекает из крови антитела, BloodToExplosiveAdapter: извлекает из крови спирт.  

Идея: Интерфейс IInputMaterial содержит метод GetVolume(). Это то, с чем могут работать все станки. Класс Blood имеет свои методы Extraction(), GetAlcohol(), GetAntibodies(). Классы-адаптеры BloodtoAmmoAdapter, BloodtoHealAdapter, BloodToExplosiveAdapter принимают объект Blood и адаптируют его методы под нужды Грейс.  



<img width="741" height="681" alt="reclass" src="https://github.com/user-attachments/assets/ee63aa5f-6187-4fef-8a1b-bbc7ac1fd7e0" />

<h3 align="center"> Реализация идеи с адаптером </h3>

```cpp
public interface class IInputMaterial {
	int GetVolume();
};

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

public ref class BloodToAmmoAdapter : IInputMaterial {
	Blood^ bloodSource;
public:
	BloodToAmmoAdapter(Blood^ b) { bloodSource = b; }
	virtual int GetVolume() { return bloodSource->ExtractIron(); }
};

public ref class BloodToHealAdapter : IInputMaterial {
	Blood^ bloodSource;
public:
	BloodToHealAdapter(Blood^ b) { bloodSource = b; }
	virtual int GetVolume() { return bloodSource->GetAlcohol(); }
};

public ref class BloodToExplosiveAdapter : IInputMaterial {
	Blood^ bloodSource;
public:
	BloodToExplosiveAdapter(Blood^ b) { bloodSource = b; }
	virtual int GetVolume() { return bloodSource->GetAntibodies(); }
};

public ref class AmmoPress {
public:
	bool Craft(IInputMaterial^ material, int count) { return material->GetVolume() >= (count * 5); }
};

public ref class MedLab {
public:
	bool Produce(IInputMaterial^ material, int count) { return material->GetVolume() >= (count * 30); }
};

public ref class ChemStation {
public:
	bool Create(IInputMaterial^ material, int count) { return material->GetVolume() >= (count * 60); }
};
```

<h3 align="center"> Реализация идеи без адаптера </h3>

```cpp
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

	public ref class AmmoPress {
	public:
		bool Craft(Blood^ blood, int count) {
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
```

