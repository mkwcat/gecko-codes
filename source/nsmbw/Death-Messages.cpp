// [Gecko]
// $Death Messages [mkwcat]
// *Adds a witty death message to the bottom left of the screen when a player
// *takes damage. Unlike what the title suggests, you don't actually have to die
// *for a message to display. That was the original idea but I never got around
// *to doing that.
// *
// *This code is long, and won't load using a standard gecko codehandler (such
// *as the one in Dolphin). To resolve this, I've created a gecko code to load
// *more gecko codes (see [Load-More-Gecko-Codes](Load-More-Gecko-Codes.md)).

#include <gct-use-cpp.h>
#include <gct.h>

#include <algorithm>
#include <array>
#include <dol/bases/d_a_player.hpp>
#include <dol/bases/d_game_com.hpp>
#include <egg/core/eggHeap.hpp>
#include <msl.h>
#include <new>
#include <numeric>
#include <nw4r/lyt/Resource.hpp>
#include <nw4r/lyt/TextBox.hpp>
#include <profiles.hpp>
#include <types.h>

GCT_ASM(
    // clang-format off

GCT_INSERT(0x801589B0, dGameDisplay_Setup)
    lwz     r3, 0x70 + 0x84(r30);
    lwz     r3, 0x4(r3);
    addi    r4, r30, 0x70 + 0x30;
    bl      dGameDisplay_InitDeathMsg;
GCT_INSERT_END(dGameDisplay_Setup)

    // clang-format on
) // GCT_ASM

struct ActorName {
    ProfileName profile;
    char name[256];
};

constexpr ActorName s_names[] = {
    {ProfileName::EN_KURIBO, "Goomba"},
    {ProfileName::EN_PATA_KURIBO, "Paragoomba"},
    {ProfileName::EN_MAME_KURIBO, "Mini Goomba"},
    {ProfileName::EN_NOKONOKO, "Koopa Troopa"},
    {ProfileName::EN_PATAPATA, "Paratroopa"},
    {ProfileName::EN_MET, "Buzzy Beetle"},
    {ProfileName::EN_TOGEZO, "Spiny"},
    {ProfileName::EN_SAKASA_TOGEZO, "Upside-down Spiny"},
    {ProfileName::EN_BUBBLE, "Lava Bubble"},
    {ProfileName::EN_DOSUN, "Thwomp"},
    {ProfileName::EN_BIGDOSUN, "Big Thwomp"},
    {ProfileName::EN_JUGEM, "Lakitu"},
    {ProfileName::EN_JUGEM_COIN, "Lakitu"},
    {ProfileName::EN_EATJUGEM, "Lakitu"},
    {ProfileName::EN_JUGEM_BODY, "Lakitu"},
    {ProfileName::EN_TOGEMET, "Spike Top"},
    {ProfileName::EN_FIREBAR, "Fire Bar"},
    {ProfileName::EN_TOGETEKKYU, "Spike Ball"},
    {ProfileName::EN_BIG_TOGETEKKYU, "Big Spike Ball"},
    {ProfileName::EN_UP_DOKAN_PAKKUN, "Piranha Plant"},
    {ProfileName::EN_DOWN_DOKAN_PAKKUN, "Piranha Plant"},
    {ProfileName::EN_RIGHT_DOKAN_PAKKUN, "Piranha Plant"},
    {ProfileName::EN_LEFT_DOKAN_PAKKUN, "Piranha Plant"},
    {ProfileName::EN_UP_DOKAN_FPAKKUN, "Fire Piranha Plant"},
    {ProfileName::EN_DOWN_DOKAN_FPAKKUN, "Fire Piranha Plant"},
    {ProfileName::EN_RIGHT_DOKAN_FPAKKUN, "Fire Piranha Plant"},
    {ProfileName::EN_LEFT_DOKAN_FPAKKUN, "Fire Piranha Plant"},
    {ProfileName::EN_JIMEN_PAKKUN, "Piranha Plant"},
    {ProfileName::EN_JIMEN_BIG_PAKKUN, "Big Guy"},
    {ProfileName::EN_JIMEN_FPAKKUN, "Fire Piranha Plant"},
    {ProfileName::EN_JIMEN_BIG_FPAKKUN, "Big Fire Guy"},
    {ProfileName::EN_WALK_PAKKUN, "Stalking Piranha Plant"},
    {ProfileName::ICEBALL, "Ice Ball"},
    {ProfileName::PL_FIREBALL, "Fireball"},
    {ProfileName::PAKKUN_FIREBALL, "Piranha Plant Fireball"},
    {ProfileName::BROS_FIREBALL, "Fire Bro Fireball"},
    {ProfileName::BOOMERANG, "Boomerang"},
    {ProfileName::EN_FIREBROS, "Fire Bro"},
    {ProfileName::EN_BOOMERANGBROS, "Boomerang Bro"},
    {ProfileName::EN_HAMMERBROS, "Hammer Bro"},
    {ProfileName::EN_LIFT_HAMMERBROS, "Hammer Bro"},
    {ProfileName::EN_ICEBROS, "Ice Bro"},
    {ProfileName::HAMMER, "Hammer"},
    {ProfileName::EN_HIMANBROS, "Sledge Bro"},
    {ProfileName::MEGA_HAMMER, "Sledge Bro Hammer"},
    {ProfileName::BROS_ICEBALL, "Ice Bro Iceball"},
    {ProfileName::EN_KILLER, "Bullet Bill"},
    {ProfileName::EN_SEARCH_KILLER, "Bullseye Bill"},
    {ProfileName::EN_MAGNUM_KILLER, "Banzai Bill"},
    {ProfileName::EN_SEARCH_MAGNUM_KILLER, "Bullseye Banzai Bill"},
    {ProfileName::EN_BASABASA, "Swoop"},
    {ProfileName::WAKI_PARABOM, "Parabomb"},
    {ProfileName::EN_PARA_BOMHEI, "Parabomb"},
    {ProfileName::EN_BOMHEI, "Bob-omb"},
    {ProfileName::EN_MECHA_KOOPA, "Mechakoopa"},
    {ProfileName::EN_MOUSE, "Scaredy Rat"},
    {ProfileName::EN_BIRIKYU, "Amp"},
    {ProfileName::EN_LINE_BIRIKYU, "Amp"},
    {ProfileName::EN_CHOROBON, "Fuzzy"},
    {ProfileName::EN_SANBO, "Pokey"},
    {ProfileName::EN_SANBO_PARTS, "Pokey"},
    {ProfileName::EN_SANBO_EL, "Pokey"},
    {ProfileName::EN_GURUGURU, "Ball and Chain"},
    {ProfileName::EN_SYNCRO_BARNAR, "Burner"},
    {ProfileName::EN_BARNAR, "Burner"},
    {ProfileName::EN_LARGE_BARNAR, "Burner"},
    {ProfileName::ROT_BARNAR, "Burner"},
    {ProfileName::EN_GESSO, "Blooper"},
    {ProfileName::EN_BARAMAKI_GESSO, "Blooper Nanny"},
    {ProfileName::EN_GESSO_CHILD, "Blooper Baby"},
    {ProfileName::EN_PUKUPUKU_PARENT, "Cheep Cheep"},
    {ProfileName::EN_PUKUPUKU, "Cheep Cheep"},
    {ProfileName::EN_TOGEPUKU, "Spiny Cheep Cheep"},
    {ProfileName::EN_MIDDLE_PUKU, "Big Cheep Cheep"},
    {ProfileName::EN_KARON, "Dry Bones"},
    {ProfileName::EN_BIGKARON, "Big Dry Bones"},
    {ProfileName::EN_NET_NOKONOKO_LR, "Climbing Koopa"},
    {ProfileName::EN_NET_NOKONOKO_UD, "Climbing Koopa"},
    {ProfileName::EN_HANACHAN, "Wiggler"},
    {ProfileName::EN_BIG_HANACHAN, "Big Wiggler"},
    {ProfileName::EN_TERESA, "Boo"},
    {ProfileName::EN_BIG_TERESA, "Big Boo"},
    {ProfileName::EN_CROW, "Crowber"},
    {ProfileName::EN_BIGPILE_UNDER, "Skewer"},
    {ProfileName::EN_BIGPILE_UPPER, "Skewer"},
    {ProfileName::EN_BIGPILE_RIGHT, "Skewer"},
    {ProfileName::EN_BIGPILE_LEFT, "Skewer"},
    {ProfileName::EN_SUPER_BIGPILE_RIGHT, "Skewer"},
    {ProfileName::EN_SUPER_BIGPILE_LEFT, "Skewer"},
    {ProfileName::EN_GOKUBUTO_BIGPILE_UNDER, "Skewer"},
    {ProfileName::EN_GOKUBUTO_BIGPILE_UPPER, "Skewer"},
    {ProfileName::EN_WANWAN, "Chain Chomp"},
    {ProfileName::EN_JUMPPUKU, "Jumping Cheep Cheep"},
    {ProfileName::EN_TOBIPUKU, "Jumping Cheep Cheep"},
    {ProfileName::EN_IGAPUKU, "Porcupuffer"},
    {ProfileName::EN_FIRESNAKE, "Fire Snake"},
    {ProfileName::EN_BOSS_KAMECK, "Kamek"},
    {ProfileName::EN_SLIP_PENGUIN, "Cooligan"},
    {ProfileName::EN_SLIP_PENGUIN2, "Cooligan"},
    {ProfileName::EN_IGA_KURIBO, "Prickly Goomba"},
    {ProfileName::KAMECK_MAGIC, "Kamek's Magic"},
    {ProfileName::EN_KERONPA, "Fire Chomp"},
    {ProfileName::KERONPA_FIRE, "Fire Chomp"},
    {ProfileName::EN_BAKUBAKU, "Cheep Chomp"},
    {ProfileName::EN_BOSS_LARRY, "Larry Koopa"},
    {ProfileName::EN_BOSS_CASTLE_LARRY, "Larry Koopa"},
    {ProfileName::OBJ_LARRY, "Larry Koopa"},
    {ProfileName::EN_BOSS_WENDY, "Wendy O. Koopa"},
    {ProfileName::EN_BOSS_CASTLE_WENDY, "Wendy O. Koopa"},
    {ProfileName::OBJ_WENDY, "Wendy O. Koopa"},
    {ProfileName::EN_BOSS_IGGY, "Iggy Koopa"},
    {ProfileName::EN_BOSS_CASTLE_IGGY, "Iggy Koopa"},
    {ProfileName::EN_BOSS_LEMMY, "Lemmy Koopa"},
    {ProfileName::EN_BOSS_CASTLE_LEMMY, "Lemmy Koopa"},
    {ProfileName::EN_BOSS_MORTON, "Morton Koopa Jr."},
    {ProfileName::EN_BOSS_CASTLE_MORTON, "Morton Koopa Jr."},
    {ProfileName::OBJ_MORTON, "Morton Koopa Jr."},
    {ProfileName::EN_BOSS_ROY, "Roy Koopa"},
    {ProfileName::EN_BOSS_CASTLE_ROY, "Roy Koopa"},
    {ProfileName::OBJ_ROY, "Roy Koopa"},
    {ProfileName::EN_BOSS_LUDWIG, "Ludwig von Koopa"},
    {ProfileName::EN_BOSS_CASTLE_LUDWIG, "Ludwig von Koopa"},
    {ProfileName::OBJ_LUDWIG, "Ludwig von Koopa"},
    {ProfileName::EN_BOSS_KOOPA, "Bowser"},
    {ProfileName::KOOPA_FIRE, "Bowser Fire Breath"},
    {ProfileName::LARRY_FIRE, "Magic Fireball"},
    {ProfileName::KOKOOPA_RING, "Wendy Ring"},
    {ProfileName::OBJ_IGGY_WANWAN, "Iggy Chain Chomp"},
    {ProfileName::CASTLE_LUDWIG_BLITZ, "Ludwig's Magic"},
    {ProfileName::FIRE_BLITZ, "Magic Fire"},
    {ProfileName::EN_UNIZOO, "Urchin"},
    {ProfileName::EN_UNIRA, "Big Urchin"},
    {ProfileName::EN_KANIBO, "Huckit Crab"},
    {ProfileName::EN_KANITAMA, "a rock"},
    {ProfileName::EN_KOPONE, "Fish Bones"},
    {ProfileName::EN_AKOYA, "Clampy"},
    {ProfileName::EN_MIDDLE_KURIBO, "Giant Goomba"},
    {ProfileName::EN_LARGE_KURIBO, "Mega Goomba"},
    {ProfileName::EN_BEANS_KURIBO, "Micro Goomba"},
    {ProfileName::JR_CLOWN_A, "Junior Clown Car"},
    {ProfileName::JR_CLOWN_B, "Junior Clown Car"},
    {ProfileName::JR_CLOWN_C, "Junior Clown Car"},
    {ProfileName::JR_CLOWN_FOR_PLAYER, "Junior Clown Car"},
    {ProfileName::BOMB_JR_C, "Bowser Jr. Bomb"},
    {ProfileName::EN_BOSS_KOOPA_JR_A, "Bowser Jr."},
    {ProfileName::EN_BOSS_KOOPA_JR_B, "Bowser Jr."},
    {ProfileName::EN_BOSS_KOOPA_JR_C, "Bowser Jr."},
    {ProfileName::JR_FIRE, "Bowser Jr. Fire Breath"},
    {ProfileName::JR_FLOOR_FIRE, "Bowser Jr. Floor Fire"},
    {ProfileName::YOGAN_INTERMITTENT, "Lava Geyser"},
    {ProfileName::EN_IBARAMUSHI, "Bramball"},
    {ProfileName::EN_CHOCHIN_ANKOH, "Bulber"},
    {ProfileName::EN_MISTMAN, "Foo"},
    {ProfileName::EN_ROT_PAKKUN, "Rotating Piranha Plant"},
    {ProfileName::EN_POLTER, "Silly Flying Thing"},
    {ProfileName::EN_ICICLE, "Icicle"},
    {ProfileName::EN_CANNON_BULLET, "Cannonball"},
    {ProfileName::KAZAN_ROCK, "Volcano Rock"},
    {ProfileName::EN_CHOROPU, "Monty Mole"},
    {ProfileName::EN_MANHOLE_CHOROPU, "Monty Mole"},
    {ProfileName::EN_JELLY_FISH, "Jellybeam"},
    {ProfileName::EN_GABON, "Spike"},
    {ProfileName::GABON_ROCK, "Spike Rock"},
    {ProfileName::EN_KING_KILLER, "King Bill"},
    {ProfileName::EN_PATAMET, "Para-Beetle"},
    {ProfileName::EN_BIG_PATAMET, "Heavy Para-Beetle"},
    {ProfileName::EN_BIG_ICICLE, "Falling Big Icicle"},
    {ProfileName::EN_BARREL, "Barrel"},
    {ProfileName::LIFT_ZEN_TOGE, "Spiky Wall from 5-Tower"},

#ifdef NON_ESSENTIAL_ACTORS
    {ProfileName::CASTLE_BOSS_DOOR, "FNAF 1 Style Door"},
    {ProfileName::CASTLE_BOSS_KEY, "Castle Boss Key"},
    {ProfileName::EN_DOOR, "Door"},
    {ProfileName::EN_SWITCHDOOR, "Door"},
    {ProfileName::EN_OBAKEDOOR, "Ghost House Door"},
    {ProfileName::EN_TORIDEDOOR, "Tower Boss Door"},
    {ProfileName::EN_CASTLEDOOR = "Castle Boss Door"},
    {ProfileName::EN_KOOPADOOR = "Final Boss Door"},
    {ProfileName::PALM_TREE = "Palm Tree"},
    {ProfileName::NICE_BOAT = "NICE_BOAT"},
    {ProfileName::LADDER = "Ladder"},
    {ProfileName::EN_REDRING, "Red Coin Ring"},
    {ProfileName::EN_SNAKEBLOCK, "Snake Block"},
    {ProfileName::SLIP_PENGUIN2_GLASSES, "Cooligan Glasses"},
#endif
};

template <u32 N>
struct PackedActorNames {
    char names[N];

    u32 getSize() const
    {
        return N;
    }
};

template <u32 P, u32 N>
constexpr PackedActorNames<P>
ProcessPackActorNames(const ActorName (&names)[N], u32& offset)
{
    PackedActorNames<P> packedNames = {};

    auto writeByte = [&](char byte) { packedNames.names[offset++] = byte; };

    // Sort names by longest to shortest
    std::array<u32, N> indices;
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&](u32 a, u32 b) {
        u32 i = 0;
        for (i = 0; names[a].name[i] != '\0' && names[b].name[i] != '\0'; i++) {
        }
        return names[a].name[i] > names[b].name[i];
    });

    for (u32 i = 0; i < N; i++) {
        const ActorName& name = names[indices[i]];

        writeByte(char(u16(name.profile) >> 2));
        writeByte(char(u16(name.profile) << 6));

        // Search back to see if we have a copy of the same name
        s32 foundOffset = -1;
        for (u32 jo = 0; jo < offset - 2; jo++) {
            for (u32 j = 0;; j++) {
                if (packedNames.names[jo + j] != name.name[j]) {
                    break;
                }
                if (name.name[j] == '\0') {
                    foundOffset = s32(jo);
                    break;
                }
            }
        }

        if (foundOffset != -1) {
            packedNames.names[offset - 1] |= 0x20 | ((foundOffset >> 8) & 0x1F);
            writeByte(foundOffset);
            continue;
        }

        u32 loc = offset++;
        for (u32 j = 0; name.name[j] != '\0'; j++) {
            writeByte(name.name[j]);
        }
        writeByte('\0');
        packedNames.names[loc] = offset - loc - 1;
    }

    return packedNames;
}

template <u32 N>
constexpr u32 GetPackedSize(const ActorName (&names)[N])
{
    u32 offset = 0;
    ProcessPackActorNames<0x1000>(names, offset);
    return offset;
}

template <u32 P, u32 N>
constexpr PackedActorNames<P> PackActorNames(const ActorName (&names)[N])
{
    u32 offset = 0;
    return ProcessPackActorNames<P>(names, offset);
}

constexpr PackedActorNames<GetPackedSize(s_names)> s_packedNames =
    PackActorNames<GetPackedSize(s_names)>(s_names);

const char* getActorFormattedName(dActor_c* actor)
{
    if (actor == nullptr) {
        return nullptr;
    }

    ProfileName name = ProfileName(actor->mProfName);
    auto names = s_packedNames.names;

    for (u32 i = 0; i < s_packedNames.getSize();) {
        u32 info = *(u32*) (names + i);
        u32 len = (info >> 8) & 0x1FFF;
        bool match = name == ProfileName(info >> 22);
        if (info & 0x200000) {
            if (match) {
                return &names[len];
            }
            i += 3;
            continue;
        }

        if (match) {
            return &names[i + 3];
        }
        i += 3 + len;
    }

    return actor->mpNameString != nullptr ? actor->mpNameString
                                          : "an unknown force";
}

nw4r::lyt::res::TextureList s_txl1 = {
    .header =
        {
            .magic = 0x74786C31, // "txl1"
            .size = sizeof(nw4r::lyt::res::TextureList),
        },
    .numEntries = 0,
    .pad = 0,
};

struct FontList1 {
    nw4r::lyt::res::FontList main;
    u32 offset1;
    nw4r::lyt::res::Font font;
    char fontName[0x20];
};

FontList1 s_fnl1 = {
    .main =
        {
            .header =
                {
                    .magic = 0x666E6C31, // "fnl1"
                    .size = sizeof(FontList1),
                },
            .numEntries = 1,
            .pad = 0,
        },
    .offset1 = offsetof(FontList1, font),
    .font =
        {
            .fontNameOffset = offsetof(FontList1, fontName),
            .pad = 0,
        },
    .fontName = "mj2d00_MessageFont_32_I4.brfnt",
};

struct MaterialList1 {
    nw4r::lyt::res::MaterialList main;
    u32 offset1;
    nw4r::lyt::res::Material material;
};

MaterialList1 s_mat1 = {
    .main =
        {
            .header =
                {
                    .magic = 0x6D617431, // "mat1"
                    .size = sizeof(MaterialList1),
                },
            .numEntries = 1,
            .pad = 0,
        },
    .offset1 = offsetof(MaterialList1, material),
    .material =
        {
            .materialName = "Mat_DeathMsg",
            .foreColor = {0x00, 0x00, 0x00, 0x00},
            .backColor = {0xFF, 0xFF, 0xFF, 0xFF},
            .colorReg3 = {0x00, 0x00, 0x00, 0x00},
            .tevColor1 = {0x00, 0x00, 0x00, 0x00},
            .tevColor2 = {0x00, 0x00, 0x00, 0x00},
            .tevColor3 = {0x00, 0x00, 0x00, 0x00},
            .tevColor4 = {0x00, 0x00, 0x00, 0x00},
            .flags = 0,
        },
};

nw4r::lyt::ResBlockSet s_resBlockSet = {
    .txl1 = &s_txl1,
    .fnl1 = &s_fnl1.main,
    .mat1 = &s_mat1.main,
    .resourceAccessor = nullptr,
};

struct TextBox1 {
    nw4r::lyt::res::TextBox main;
    wchar_t message[128];
};

nw4r::lyt::res::TextBox s_txt1 = {
    .pane =
        {
            .header =
                {
                    .magic = 0x74787431, // "txt1"
                    .size = sizeof(TextBox1),
                },

            .flags = 0x5,
            .originType = 0,
            .alpha = 0xFF,
            .pad_0xB = 0,
            .name = "Txt_DeathMsg",
            .userData = {},
            .translation = {-250.0, 0.0, 0.0},
            .rotation = {0.0, 0.0, 0.0},
            .scale = {1.0, 1.0},
            .width = 800.0,
            .height = 45.0,
        },

    .stringSize = 127,
    .maxStringSize = 127,
    .matIndex = 0,
    .fontIndex = 0,
    .stringOrigin = 0,
    .lineAlignment = 2,
    .pad_0x56 = 0,
    .textOffset = offsetof(TextBox1, message),
    .topColor = {0xFF, 0x7F, 0x7F, 0xFF},
    .bottomColor = {0xFF, 0x30, 0x30, 0xFF},
    .fontSizeX = 25.0,
    .fontSizeY = 25.0,
    .characterSize = 1.0,
    .lineSize = 1.0,
};

void* s_drawInfo = nullptr;

const char* const s_playerNames[4] = {
    "Mario",
    "Luigi",
    "Bload",
    "Yoad",
};

const u32 s_playerColors[4] = {
    0xFF7F7FFF,
    0x7FFF7FFF,
    0x7F7FFFFF,
    0xFFFF7FFF,
};

class DeathMsgMgr
{
public:
    static constexpr u32 MaxMessages = 12;

    void NewMessage(nw4r::lyt::res::TextBox* res)
    {
        if (m_count >= MaxMessages) {
            DeleteFromFront();
        }

        u32 index = (m_index + m_count) % MaxMessages;
        m_count++;

        new (&u.m_textBox[index]) nw4r::lyt::TextBox(res, s_resBlockSet);
        m_timeToLive[index] = 8 * 60;
    }

    void DeleteFromFront()
    {
        u.m_textBox[m_index].__dt(0);
        m_index = (m_index + 1) % MaxMessages;
        m_count--;
    }

    union U {
        u8 m_data[sizeof(nw4r::lyt::TextBox) * MaxMessages];
        nw4r::lyt::TextBox m_textBox[MaxMessages];

        U()
        {
            // Prevent calling constructor yet
            (void) m_data;
        }
    } u;

    u32 m_index;
    u32 m_count;
    u32 m_timeToLive[MaxMessages];
};

DeathMsgMgr* s_deathMsgMgr = nullptr;

void dAcPy_c::setDeathMessage(const char* message, const char* enemy)
{
    if (message == nullptr || message[0] == '\0') {
        return;
    }

    TextBox1 textBoxRes = {};
    s32 player = this->mCharacter < 4 ? this->mCharacter : 0;

    memcpy(&textBoxRes.main, &s_txt1, sizeof(nw4r::lyt::res::TextBox));
    *(u32*) &textBoxRes.main.topColor = s_playerColors[player];
    *(u32*) &textBoxRes.main.bottomColor = s_playerColors[player];

    char formatted[128] = {0};
    snprintf(
        formatted, sizeof(formatted), message, s_playerNames[player], enemy
    );

    mbstowcs(textBoxRes.message, formatted, 128);

    s_deathMsgMgr->NewMessage(&textBoxRes.main);
}

cGCT_INSERT_POINTER( //
    0x80325BB8
) bool dAcPy_c::setDamage2_Override(dActor_c* source, DamageType_e type)
{
    const char* enemy = getActorFormattedName(source);

    if (dAcPy_c::sDeathMessageOverride != nullptr) {
        setDeathMessage(dAcPy_c::sDeathMessageOverride, enemy);
        dAcPy_c::sDeathMessageOverride = nullptr;

        return dAcPy_c::setDamage2__IMPL(source, type);
    }

    switch (type) {
    case DAMAGE_KNOCKBACK_LONG:
    case DAMAGE_KNOCKBACK_LONG2:
    case DAMAGE_KNOCKBACK_SHORT:
    case DAMAGE_KNOCKBACK_SHORT2:
    case DAMAGE_BOUNCE:
        return dAcPy_c::setDamage2__IMPL(source, type);
    }

    const char* message = nullptr;

    switch (type) {
    default:
        if (enemy == nullptr) {
            message = "%s came into contact with something deadly";
            break;
        }

        switch (ProfileName(source->mProfName)) {
        default:
            message = (const char*[]){
                "%s came into contact with %s", "%s lost it to %s",
                "%s tried to give %s a hug",    "%s lost a fight with %s",
                "%s reached an impassable %s",
            }[dGameCom_c::getRandom(enemy[0] == 'a' ? 4 : 5)];
            break;

        case ProfileName::EN_BIGPILE_UNDER:
        case ProfileName::EN_BIGPILE_UPPER:
        case ProfileName::EN_BIGPILE_RIGHT:
        case ProfileName::EN_BIGPILE_LEFT:
        case ProfileName::EN_SUPER_BIGPILE_RIGHT:
        case ProfileName::EN_SUPER_BIGPILE_LEFT:
        case ProfileName::EN_GOKUBUTO_BIGPILE_UNDER:
        case ProfileName::EN_GOKUBUTO_BIGPILE_UPPER:
            message = "%s was penetrated by %s";
            break;

        case ProfileName::EN_KILLER:
        case ProfileName::EN_SEARCH_KILLER:
        case ProfileName::EN_MAGNUM_KILLER:
        case ProfileName::EN_SEARCH_MAGNUM_KILLER:
            message = "%s was shot by %s";
            break;

        case ProfileName::EN_KING_KILLER:
            message = "%s was blasted by %s";
            break;

        case ProfileName::EN_ICICLE:
        case ProfileName::EN_BIG_ICICLE:
            message = "%s was impaled by %s";
            break;
        }
        break;

    case DAMAGE_ELEC_SHOCK:
        if (enemy == nullptr) {
            message = "%s was electrocuted";
        } else {
            message = "%s was electrocuted by %s";
        }
        break;

    case DAMAGE_LAVA:
        message = "%s tried to swim in lava";
        break;

    case DAMAGE_POISON_WATER:
        message = "%s tried to drink the weird lanky water";
        break;

    case DAMAGE_CRUSH:
        if (enemy == nullptr) {
            message = "%s was crushed by a heavy object";
        } else {
            message = "%s was squished too much by %s";
        }
        break;

    case DAMAGE_EAT_DIE:
    case DAMAGE_EAT_DIE2:
        if (enemy == nullptr) {
            message = "%s was eaten by a hungry creature";
        } else {
            message = "%s was eaten by %s";
        }
        break;

    case DAMAGE_FREEZE:
    case DAMAGE_FREEZE2:
        if (enemy == nullptr) {
            message = "%s was frozen solid";
        } else {
            message = "%s was frozen solid by %s";
        }
        break;

    case DAMAGE_POISON_FOG:
        message = "%s couldn't escape the fog";
        break;
    }

    setDeathMessage(message, enemy);

    return dAcPy_c::setDamage2__IMPL(source, type);
}

cGCT_INSERT_POINTER( //
    0x80325884
) void dAcPy_c::setBalloonInDispOut_Override(int type)
{
    char message[128] = {0};
    const char* name = s_playerNames[this->mCharacter];

    switch (type) {
    default:
        snprintf(
            message, sizeof(message), "%s fell out of the world (type %d)",
            name, type
        );
        break;

    case 2:
        snprintf(message, sizeof(message), "%s was left behind", name);
        snprintf(
            message, sizeof(message),
            (const char*[]){
                "%s was left behind",
                "%s mysteriously vanished",
                "%s was crushed between the screen and a heavy object",
            }[dGameCom_c::getRandom(3)],
            name
        );
        break;

    case 3:
        snprintf(
            message, sizeof(message),
            (const char*[]){
                "%s fell out of the world",
                "%s mysteriously vanished",
            }[dGameCom_c::getRandom(2)],
            name
        );
        break;
    }

    setDeathMessage(message, "");

    dAcPy_c::setBalloonInDispOut__IMPL(type);
}

extern "C" bool
dGameDisplay_InitDeathMsg(void* resourceAccessor, void* drawInfo)
{
    s_deathMsgMgr = new DeathMsgMgr();
    s_resBlockSet.resourceAccessor = resourceAccessor;
    s_drawInfo = drawInfo;

    return true;
}

cGCT_IMPORT(0x802ABBA0
) void Pane_CalculateMtx(nw4r::lyt::Pane* pane, void* drawInfo);
cGCT_IMPORT(0x802ABEA0) void Pane_Draw(nw4r::lyt::Pane* pane, void* drawInfo);

cGCT_INSERT_BL( //
    0x809241DC
) u32 drawDeathMsgs()
{
    auto mgr = s_deathMsgMgr;
    u32 count = mgr->m_count;

    float position = -170.0 + 35.0 * count;
    void* drawInfo = s_drawInfo;

    for (s32 i = 0; i < count; i++) {
        position -= 35.0;
        u32 index = (mgr->m_index + i) % mgr->MaxMessages;
        auto textBox = &mgr->u.m_textBox[index];

        u32 ttl = --mgr->m_timeToLive[index];
        textBox->m_alpha = std::min<u32>(ttl * 255 / 30, 255);

        textBox->m_translation[1] = position;

        Pane_CalculateMtx(textBox, drawInfo);
        Pane_Draw(textBox, drawInfo);

        if (ttl == 0) {
            mgr->DeleteFromFront();
            count--;
            i--;
        }
    }

    return 0;
}