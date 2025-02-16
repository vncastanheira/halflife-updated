#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"

LINK_ENTITY_TO_CLASS(weapon_ak47, CAK47);

void CAK47::Spawn()
{
    Precache();
    SET_MODEL(ENT(pev), "models/w_ak47.mdl");
    m_iId = WEAPON_AK47;
    m_iDefaultAmmo = 30; // How much ammo this weapon has on spawn
    FallInit(); // get ready to fall down.
}

void CAK47::Precache()
{
    PRECACHE_MODEL("models/v_ak47.mdl");
    PRECACHE_MODEL("models/w_ak47.mdl");
    PRECACHE_MODEL("models/p_ak47.mdl");

    m_iShell = PRECACHE_MODEL("models/shell.mdl"); // brass shell

    PRECACHE_SOUND("weapons/ak47-1.wav");
}

bool CAK47::GetItemInfo(ItemInfo* p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = "762"; // Which ammo type this weapon use
    p->iMaxAmmo1 = 90; // What's the max ammo quantity for that kind of ammo
    p->pszAmmo2 = NULL;
    p->iMaxAmmo2 = NULL;
    p->iMaxClip = 30; // How many ammo this weapon's clip or magazine has
    p->iSlot = 2; // Which "slot" (column) in the HUD this weapon is located (2 = same slot as HL1 MP5, shotgun, crossbow)
    p->iPosition = 3; // Which "position" (row) in the HUD this weapon is located (3 = after HL1 crossbow)
    p->iFlags = 0; // Special flags this weapon has
    p->iId = m_iId = WEAPON_AK47;
    p->iWeight = MP5_WEIGHT; // How much "priority" this weapon has when auto-switch is triggered

    return true;
}

bool CAK47::Deploy()
{
    //  The last parameter is the animation set for the player model in thirdperson to use
    return DefaultDeploy("models/v_ak47.mdl", "models/p_ak47.mdl", AK47_DRAW, "mp5");
}

void CAK47::PrimaryAttack()
{
    // Size of the muzzle flash and how much volume in the world the firing sound produces
    m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
    m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
    m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

    // player "shoot" animation
    m_pPlayer->SetAnimation(PLAYER_ATTACK1);

    Vector vecSrc = m_pPlayer->GetGunPosition();
    Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
    // Bullet firing code. By order:
    // 1. No. of bullet: 1 for other guns, multiple for shotgun)
    // 2. Firing source: its firing from player 3rd person mdl's gun attachment point
    // 3. Firing direction: The autoaim vector is mainly for autoaim feature, however it's still used to pinpoint end position
    // 4. Spread: How big the randomness of the bullet spread. To make 100% accuracy, make it Vector(0, 0, 0). Otherwise, you can use vector cone
        // 5. Trace line end position: 8192 is the max world length in HL
    // 6. Bullet type: Control end point effects and damage. If damage is not specified in slot number 8 below, it will use default value of MP5 damage.
    // 7. Frequency of tracer effect. Has a number between 0-4. 0 means no tracer at all (like in CS 1.6), 1 is the highest frequency.
    // 8. Damage. If left empty, it will use the default of value of bullet type.
    m_pPlayer->FireBullets(1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, 8192, BULLET_PLAYER_MP5, 1, 30);

    // Play view model animation and firing sound
    SendWeaponAnim(AK47_SHOOT1 + RANDOM_LONG(0, 2));
    // the 3rd entry defines the firing sound path. Make sure to precache first
    EMIT_SOUND(m_pPlayer->edict(), CHAN_WEAPON, "weapons/ak47-1.wav", 1, ATTN_NORM);

    // Eject the brass
    Vector vecShellVelocity = m_pPlayer->pev->velocity + gpGlobals->v_right * RANDOM_FLOAT(100, 200) +
        gpGlobals->v_up * RANDOM_FLOAT(100, 150) + gpGlobals->v_forward * 25;
    EjectBrass(pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -12 + gpGlobals->v_forward * 20 +
        gpGlobals->v_right * -8, vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL);

    // Punch the camera to simulate recoil
    m_pPlayer->pev->punchangle.x -= 2;
    // Remove a bullet
    m_iClip--;
    // Next time for attack and weapon idling
    m_flNextPrimaryAttack = 0.1;
    m_flTimeWeaponIdle = 0.85;
}

void CAK47::Reload()
{
    // If ammo is not 30, reload using AK47_RELOAD animation and disable firing and idling for 2.5 seconds
    DefaultReload(30, AK47_RELOAD, 2.5);
}


void CAK47::WeaponIdle()
{
    ResetEmptySound();

    // Not the time to idle
    if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
        return;

    // Play idle animation
    SendWeaponAnim(AK47_IDLE);
    // When to call idle animation again in seconds. Change this value if your mdl has a certain value for idle animation for smooth transitioning
    m_flTimeWeaponIdle = 10;
}