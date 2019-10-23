#ifndef WEAPON_H
#define WEAPON_H

struct Weapon
{
	int damage = 100;
	float fireSpeed = 3.0f;
	float bulletSpeed = 4.0f;
	float bulletLifetime = 2.0f;
};

class VehicleWeapon
{
public:
	static constexpr Weapon defaultWeapon = Weapon();

	                                        //{ damage, fireSpeed, bulletSpeed, bulletLifetime };
	static constexpr Weapon machineGun =      {    120,      0.1f,        10.0f,           2.0f };
	static constexpr Weapon missileLauncher = {    200,      1.5f,        3.0f,           3.0f };
	static constexpr Weapon laser =           {    160,      1.0f,       10.0f,           0.5f };
	static constexpr Weapon flameThrower =    {     80,      0.1f,        2.0f,           1.5f };
};

class AIWeapon
{
public:
	static constexpr Weapon defaultWeapon = Weapon();

											//{  damage, fireSpeed, bulletSpeed, bulletLifetime };
	static constexpr Weapon machineGun =	  {		  2,      0.5f,       30.0f,           2.0f };
};

#endif // !WEAPON_H