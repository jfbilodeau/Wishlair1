//
// Created by jfbil on 2023-06-27.
//

#ifndef NOMAD_INPUTMANAGER_HPP
#define NOMAD_INPUTMANAGER_HPP

#include "nomad/nomad.hpp"

#include "SDL_scancode.h"
#include "SDL_events.h"

#include <vector>

namespace nomad {

// Forward declarations
class Game;

using InputCode = NomadId;

// Nomad input mapping.
const InputCode INPUT_CODE_NONE = -1;
const InputCode INPUT_KEYBOARD_OFFSET = 0;
const InputCode INPUT_MOUSE_OFFSET = 1000;
const InputCode INPUT_CONTROLLER1_OFFSET = 2000;

// Based on SDL2's SDL_scancode.h
// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_scancode.h
const InputCode INPUT_SCANCODE_UNKNOWN = INPUT_KEYBOARD_OFFSET + 0;
const InputCode INPUT_SCANCODE_A = INPUT_KEYBOARD_OFFSET + 4;
const InputCode INPUT_SCANCODE_B = INPUT_KEYBOARD_OFFSET + 5;
const InputCode INPUT_SCANCODE_C = INPUT_KEYBOARD_OFFSET + 6;
const InputCode INPUT_SCANCODE_D = INPUT_KEYBOARD_OFFSET + 7;
const InputCode INPUT_SCANCODE_E = INPUT_KEYBOARD_OFFSET + 8;
const InputCode INPUT_SCANCODE_F = INPUT_KEYBOARD_OFFSET + 9;
const InputCode INPUT_SCANCODE_G = INPUT_KEYBOARD_OFFSET + 10;
const InputCode INPUT_SCANCODE_H = INPUT_KEYBOARD_OFFSET + 11;
const InputCode INPUT_SCANCODE_I = INPUT_KEYBOARD_OFFSET + 12;
const InputCode INPUT_SCANCODE_J = INPUT_KEYBOARD_OFFSET + 13;
const InputCode INPUT_SCANCODE_K = INPUT_KEYBOARD_OFFSET + 14;
const InputCode INPUT_SCANCODE_L = INPUT_KEYBOARD_OFFSET + 15;
const InputCode INPUT_SCANCODE_M = INPUT_KEYBOARD_OFFSET + 16;
const InputCode INPUT_SCANCODE_N = INPUT_KEYBOARD_OFFSET + 17;
const InputCode INPUT_SCANCODE_O = INPUT_KEYBOARD_OFFSET + 18;
const InputCode INPUT_SCANCODE_P = INPUT_KEYBOARD_OFFSET + 19;
const InputCode INPUT_SCANCODE_Q = INPUT_KEYBOARD_OFFSET + 20;
const InputCode INPUT_SCANCODE_R = INPUT_KEYBOARD_OFFSET + 21;
const InputCode INPUT_SCANCODE_S = INPUT_KEYBOARD_OFFSET + 22;
const InputCode INPUT_SCANCODE_T = INPUT_KEYBOARD_OFFSET + 23;
const InputCode INPUT_SCANCODE_U = INPUT_KEYBOARD_OFFSET + 24;
const InputCode INPUT_SCANCODE_V = INPUT_KEYBOARD_OFFSET + 25;
const InputCode INPUT_SCANCODE_W = INPUT_KEYBOARD_OFFSET + 26;
const InputCode INPUT_SCANCODE_X = INPUT_KEYBOARD_OFFSET + 27;
const InputCode INPUT_SCANCODE_Y = INPUT_KEYBOARD_OFFSET + 28;
const InputCode INPUT_SCANCODE_Z = INPUT_KEYBOARD_OFFSET + 29;
const InputCode INPUT_SCANCODE_1 = INPUT_KEYBOARD_OFFSET + 30;
const InputCode INPUT_SCANCODE_2 = INPUT_KEYBOARD_OFFSET + 31;
const InputCode INPUT_SCANCODE_3 = INPUT_KEYBOARD_OFFSET + 32;
const InputCode INPUT_SCANCODE_4 = INPUT_KEYBOARD_OFFSET + 33;
const InputCode INPUT_SCANCODE_5 = INPUT_KEYBOARD_OFFSET + 34;
const InputCode INPUT_SCANCODE_6 = INPUT_KEYBOARD_OFFSET + 35;
const InputCode INPUT_SCANCODE_7 = INPUT_KEYBOARD_OFFSET + 36;
const InputCode INPUT_SCANCODE_8 = INPUT_KEYBOARD_OFFSET + 37;
const InputCode INPUT_SCANCODE_9 = INPUT_KEYBOARD_OFFSET + 38;
const InputCode INPUT_SCANCODE_0 = INPUT_KEYBOARD_OFFSET + 39;
const InputCode INPUT_SCANCODE_RETURN = INPUT_KEYBOARD_OFFSET + 40;
const InputCode INPUT_SCANCODE_ESCAPE = INPUT_KEYBOARD_OFFSET + 41;
const InputCode INPUT_SCANCODE_BACKSPACE = INPUT_KEYBOARD_OFFSET + 42;
const InputCode INPUT_SCANCODE_TAB = INPUT_KEYBOARD_OFFSET + 43;
const InputCode INPUT_SCANCODE_SPACE = INPUT_KEYBOARD_OFFSET + 44;
const InputCode INPUT_SCANCODE_MINUS = INPUT_KEYBOARD_OFFSET + 45;
const InputCode INPUT_SCANCODE_EQUALS = INPUT_KEYBOARD_OFFSET + 46;
const InputCode INPUT_SCANCODE_LEFTBRACKET = INPUT_KEYBOARD_OFFSET + 47;
const InputCode INPUT_SCANCODE_RIGHTBRACKET = INPUT_KEYBOARD_OFFSET + 48;
const InputCode INPUT_SCANCODE_BACKSLASH = INPUT_KEYBOARD_OFFSET + 49;
const InputCode INPUT_SCANCODE_NONUSHASH = INPUT_KEYBOARD_OFFSET + 50;
const InputCode INPUT_SCANCODE_SEMICOLON = INPUT_KEYBOARD_OFFSET + 51;
const InputCode INPUT_SCANCODE_APOSTROPHE = INPUT_KEYBOARD_OFFSET + 52;
const InputCode INPUT_SCANCODE_GRAVE = INPUT_KEYBOARD_OFFSET + 53;
const InputCode INPUT_SCANCODE_COMMA = INPUT_KEYBOARD_OFFSET + 54;
const InputCode INPUT_SCANCODE_PERIOD = INPUT_KEYBOARD_OFFSET + 55;
const InputCode INPUT_SCANCODE_SLASH = INPUT_KEYBOARD_OFFSET + 56;
const InputCode INPUT_SCANCODE_CAPSLOCK = INPUT_KEYBOARD_OFFSET + 57;
const InputCode INPUT_SCANCODE_F1 = INPUT_KEYBOARD_OFFSET + 58;
const InputCode INPUT_SCANCODE_F2 = INPUT_KEYBOARD_OFFSET + 59;
const InputCode INPUT_SCANCODE_F3 = INPUT_KEYBOARD_OFFSET + 60;
const InputCode INPUT_SCANCODE_F4 = INPUT_KEYBOARD_OFFSET + 61;
const InputCode INPUT_SCANCODE_F5 = INPUT_KEYBOARD_OFFSET + 62;
const InputCode INPUT_SCANCODE_F6 = INPUT_KEYBOARD_OFFSET + 63;
const InputCode INPUT_SCANCODE_F7 = INPUT_KEYBOARD_OFFSET + 64;
const InputCode INPUT_SCANCODE_F8 = INPUT_KEYBOARD_OFFSET + 65;
const InputCode INPUT_SCANCODE_F9 = INPUT_KEYBOARD_OFFSET + 66;
const InputCode INPUT_SCANCODE_F10 = INPUT_KEYBOARD_OFFSET + 67;
const InputCode INPUT_SCANCODE_F11 = INPUT_KEYBOARD_OFFSET + 68;
const InputCode INPUT_SCANCODE_F12 = INPUT_KEYBOARD_OFFSET + 69;
const InputCode INPUT_SCANCODE_PRINTSCREEN = INPUT_KEYBOARD_OFFSET + 70;
const InputCode INPUT_SCANCODE_SCROLLLOCK = INPUT_KEYBOARD_OFFSET + 71;
const InputCode INPUT_SCANCODE_PAUSE = INPUT_KEYBOARD_OFFSET + 72;
const InputCode INPUT_SCANCODE_INSERT = INPUT_KEYBOARD_OFFSET + 73;
const InputCode INPUT_SCANCODE_HOME = INPUT_KEYBOARD_OFFSET + 74;
const InputCode INPUT_SCANCODE_PAGEUP = INPUT_KEYBOARD_OFFSET + 75;
const InputCode INPUT_SCANCODE_DELETE = INPUT_KEYBOARD_OFFSET + 76;
const InputCode INPUT_SCANCODE_END = INPUT_KEYBOARD_OFFSET + 77;
const InputCode INPUT_SCANCODE_PAGEDOWN = INPUT_KEYBOARD_OFFSET + 78;
const InputCode INPUT_SCANCODE_RIGHT = INPUT_KEYBOARD_OFFSET + 79;
const InputCode INPUT_SCANCODE_LEFT = INPUT_KEYBOARD_OFFSET + 80;
const InputCode INPUT_SCANCODE_DOWN = INPUT_KEYBOARD_OFFSET + 81;
const InputCode INPUT_SCANCODE_UP = INPUT_KEYBOARD_OFFSET + 82;
const InputCode INPUT_SCANCODE_NUMLOCKCLEAR = INPUT_KEYBOARD_OFFSET + 83;
const InputCode INPUT_SCANCODE_KP_DIVIDE = INPUT_KEYBOARD_OFFSET + 84;
const InputCode INPUT_SCANCODE_KP_MULTIPLY = INPUT_KEYBOARD_OFFSET + 85;
const InputCode INPUT_SCANCODE_KP_MINUS = INPUT_KEYBOARD_OFFSET + 86;
const InputCode INPUT_SCANCODE_KP_PLUS = INPUT_KEYBOARD_OFFSET + 87;
const InputCode INPUT_SCANCODE_KP_ENTER = INPUT_KEYBOARD_OFFSET + 88;
const InputCode INPUT_SCANCODE_KP_1 = INPUT_KEYBOARD_OFFSET + 89;
const InputCode INPUT_SCANCODE_KP_2 = INPUT_KEYBOARD_OFFSET + 90;
const InputCode INPUT_SCANCODE_KP_3 = INPUT_KEYBOARD_OFFSET + 91;
const InputCode INPUT_SCANCODE_KP_4 = INPUT_KEYBOARD_OFFSET + 92;
const InputCode INPUT_SCANCODE_KP_5 = INPUT_KEYBOARD_OFFSET + 93;
const InputCode INPUT_SCANCODE_KP_6 = INPUT_KEYBOARD_OFFSET + 94;
const InputCode INPUT_SCANCODE_KP_7 = INPUT_KEYBOARD_OFFSET + 95;
const InputCode INPUT_SCANCODE_KP_8 = INPUT_KEYBOARD_OFFSET + 96;
const InputCode INPUT_SCANCODE_KP_9 = INPUT_KEYBOARD_OFFSET + 97;
const InputCode INPUT_SCANCODE_KP_0 = INPUT_KEYBOARD_OFFSET + 98;
const InputCode INPUT_SCANCODE_KP_PERIOD = INPUT_KEYBOARD_OFFSET + 99;
const InputCode INPUT_SCANCODE_NONUSBACKSLASH = INPUT_KEYBOARD_OFFSET + 100;
const InputCode INPUT_SCANCODE_APPLICATION = INPUT_KEYBOARD_OFFSET + 101;
const InputCode INPUT_SCANCODE_POWER = INPUT_KEYBOARD_OFFSET + 102;
const InputCode INPUT_SCANCODE_KP_EQUALS = INPUT_KEYBOARD_OFFSET + 103;
const InputCode INPUT_SCANCODE_F13 = INPUT_KEYBOARD_OFFSET + 104;
const InputCode INPUT_SCANCODE_F14 = INPUT_KEYBOARD_OFFSET + 105;
const InputCode INPUT_SCANCODE_F15 = INPUT_KEYBOARD_OFFSET + 106;
const InputCode INPUT_SCANCODE_F16 = INPUT_KEYBOARD_OFFSET + 107;
const InputCode INPUT_SCANCODE_F17 = INPUT_KEYBOARD_OFFSET + 108;
const InputCode INPUT_SCANCODE_F18 = INPUT_KEYBOARD_OFFSET + 109;
const InputCode INPUT_SCANCODE_F19 = INPUT_KEYBOARD_OFFSET + 110;
const InputCode INPUT_SCANCODE_F20 = INPUT_KEYBOARD_OFFSET + 111;
const InputCode INPUT_SCANCODE_F21 = INPUT_KEYBOARD_OFFSET + 112;
const InputCode INPUT_SCANCODE_F22 = INPUT_KEYBOARD_OFFSET + 113;
const InputCode INPUT_SCANCODE_F23 = INPUT_KEYBOARD_OFFSET + 114;
const InputCode INPUT_SCANCODE_F24 = INPUT_KEYBOARD_OFFSET + 115;
const InputCode INPUT_SCANCODE_EXECUTE = INPUT_KEYBOARD_OFFSET + 116;
const InputCode INPUT_SCANCODE_HELP = INPUT_KEYBOARD_OFFSET + 117;
const InputCode INPUT_SCANCODE_MENU = INPUT_KEYBOARD_OFFSET + 118;
const InputCode INPUT_SCANCODE_SELECT = INPUT_KEYBOARD_OFFSET + 119;
const InputCode INPUT_SCANCODE_STOP = INPUT_KEYBOARD_OFFSET + 120;
const InputCode INPUT_SCANCODE_AGAIN = INPUT_KEYBOARD_OFFSET + 121;
const InputCode INPUT_SCANCODE_UNDO = INPUT_KEYBOARD_OFFSET + 122;
const InputCode INPUT_SCANCODE_CUT = INPUT_KEYBOARD_OFFSET + 123;
const InputCode INPUT_SCANCODE_COPY = INPUT_KEYBOARD_OFFSET + 124;
const InputCode INPUT_SCANCODE_PASTE = INPUT_KEYBOARD_OFFSET + 125;
const InputCode INPUT_SCANCODE_FIND = INPUT_KEYBOARD_OFFSET + 126;
const InputCode INPUT_SCANCODE_MUTE = INPUT_KEYBOARD_OFFSET + 127;
const InputCode INPUT_SCANCODE_VOLUMEUP = INPUT_KEYBOARD_OFFSET + 128;
const InputCode INPUT_SCANCODE_VOLUMEDOWN = INPUT_KEYBOARD_OFFSET + 129;
// not sure whether there's a reason to enable these
// const InputCode INPUT_SCANCODE_LOCKINGCAPSLOCK = INPUT_KEYBOARD_OFFSET + 130;
// const InputCode INPUT_SCANCODE_LOCKINGNUMLOCK = INPUT_KEYBOARD_OFFSET + 131;
// const InputCode INPUT_SCANCODE_LOCKINGSCROLLLOCK = INPUT_KEYBOARD_OFFSET + 132;
const InputCode INPUT_SCANCODE_KP_COMMA = INPUT_KEYBOARD_OFFSET + 133;
const InputCode INPUT_SCANCODE_KP_EQUALSAS400 = INPUT_KEYBOARD_OFFSET + 134;
const InputCode INPUT_SCANCODE_INTERNATIONAL1 = INPUT_KEYBOARD_OFFSET + 135;
const InputCode INPUT_SCANCODE_INTERNATIONAL2 = INPUT_KEYBOARD_OFFSET + 136;
const InputCode INPUT_SCANCODE_INTERNATIONAL3 = INPUT_KEYBOARD_OFFSET + 137;
const InputCode INPUT_SCANCODE_INTERNATIONAL4 = INPUT_KEYBOARD_OFFSET + 138;
const InputCode INPUT_SCANCODE_INTERNATIONAL5 = INPUT_KEYBOARD_OFFSET + 139;
const InputCode INPUT_SCANCODE_INTERNATIONAL6 = INPUT_KEYBOARD_OFFSET + 140;
const InputCode INPUT_SCANCODE_INTERNATIONAL7 = INPUT_KEYBOARD_OFFSET + 141;
const InputCode INPUT_SCANCODE_INTERNATIONAL8 = INPUT_KEYBOARD_OFFSET + 142;
const InputCode INPUT_SCANCODE_INTERNATIONAL9 = INPUT_KEYBOARD_OFFSET + 143;
const InputCode INPUT_SCANCODE_LANG1 = INPUT_KEYBOARD_OFFSET + 144; // Hangul/English toggle
const InputCode INPUT_SCANCODE_LANG2 = INPUT_KEYBOARD_OFFSET + 145; // Hanja conversion
const InputCode INPUT_SCANCODE_LANG3 = INPUT_KEYBOARD_OFFSET + 146; // Katakana
const InputCode INPUT_SCANCODE_LANG4 = INPUT_KEYBOARD_OFFSET + 147; // Hiragana
const InputCode INPUT_SCANCODE_LANG5 = INPUT_KEYBOARD_OFFSET + 148; // Zenkaku/Hankaku
const InputCode INPUT_SCANCODE_LANG6 = INPUT_KEYBOARD_OFFSET + 149; // reserved
const InputCode INPUT_SCANCODE_LANG7 = INPUT_KEYBOARD_OFFSET + 150; // reserved
const InputCode INPUT_SCANCODE_LANG8 = INPUT_KEYBOARD_OFFSET + 151; // reserved
const InputCode INPUT_SCANCODE_LANG9 = INPUT_KEYBOARD_OFFSET + 152; // reserved
const InputCode INPUT_SCANCODE_ALTERASE = INPUT_KEYBOARD_OFFSET + 153;
const InputCode INPUT_SCANCODE_SYSREQ = INPUT_KEYBOARD_OFFSET + 154;
const InputCode INPUT_SCANCODE_CANCEL = INPUT_KEYBOARD_OFFSET + 155;
const InputCode INPUT_SCANCODE_CLEAR = INPUT_KEYBOARD_OFFSET + 156;
const InputCode INPUT_SCANCODE_PRIOR = INPUT_KEYBOARD_OFFSET + 157;
const InputCode INPUT_SCANCODE_RETURN2 = INPUT_KEYBOARD_OFFSET + 158;
const InputCode INPUT_SCANCODE_SEPARATOR = INPUT_KEYBOARD_OFFSET + 159;
const InputCode INPUT_SCANCODE_OUT = INPUT_KEYBOARD_OFFSET + 160;
const InputCode INPUT_SCANCODE_OPER = INPUT_KEYBOARD_OFFSET + 161;
const InputCode INPUT_SCANCODE_CLEARAGAIN = INPUT_KEYBOARD_OFFSET + 162;
const InputCode INPUT_SCANCODE_CRSEL = INPUT_KEYBOARD_OFFSET + 163;
const InputCode INPUT_SCANCODE_EXSEL = INPUT_KEYBOARD_OFFSET + 164;
const InputCode INPUT_SCANCODE_KP_00 = INPUT_KEYBOARD_OFFSET + 176;
const InputCode INPUT_SCANCODE_KP_000 = INPUT_KEYBOARD_OFFSET + 177;
const InputCode INPUT_SCANCODE_THOUSANDSSEPARATOR = INPUT_KEYBOARD_OFFSET + 178;
const InputCode INPUT_SCANCODE_DECIMALSEPARATOR = INPUT_KEYBOARD_OFFSET + 179;
const InputCode INPUT_SCANCODE_CURRENCYUNIT = INPUT_KEYBOARD_OFFSET + 180;
const InputCode INPUT_SCANCODE_CURRENCYSUBUNIT = INPUT_KEYBOARD_OFFSET + 181;
const InputCode INPUT_SCANCODE_KP_LEFTPAREN = INPUT_KEYBOARD_OFFSET + 182;
const InputCode INPUT_SCANCODE_KP_RIGHTPAREN = INPUT_KEYBOARD_OFFSET + 183;
const InputCode INPUT_SCANCODE_KP_LEFTBRACE = INPUT_KEYBOARD_OFFSET + 184;
const InputCode INPUT_SCANCODE_KP_RIGHTBRACE = INPUT_KEYBOARD_OFFSET + 185;
const InputCode INPUT_SCANCODE_KP_TAB = INPUT_KEYBOARD_OFFSET + 186;
const InputCode INPUT_SCANCODE_KP_BACKSPACE = INPUT_KEYBOARD_OFFSET + 187;
const InputCode INPUT_SCANCODE_KP_A = INPUT_KEYBOARD_OFFSET + 188;
const InputCode INPUT_SCANCODE_KP_B = INPUT_KEYBOARD_OFFSET + 189;
const InputCode INPUT_SCANCODE_KP_C = INPUT_KEYBOARD_OFFSET + 190;
const InputCode INPUT_SCANCODE_KP_D = INPUT_KEYBOARD_OFFSET + 191;
const InputCode INPUT_SCANCODE_KP_E = INPUT_KEYBOARD_OFFSET + 192;
const InputCode INPUT_SCANCODE_KP_F = INPUT_KEYBOARD_OFFSET + 193;
const InputCode INPUT_SCANCODE_KP_XOR = INPUT_KEYBOARD_OFFSET + 194;
const InputCode INPUT_SCANCODE_KP_POWER = INPUT_KEYBOARD_OFFSET + 195;
const InputCode INPUT_SCANCODE_KP_PERCENT = INPUT_KEYBOARD_OFFSET + 196;
const InputCode INPUT_SCANCODE_KP_LESS = INPUT_KEYBOARD_OFFSET + 197;
const InputCode INPUT_SCANCODE_KP_GREATER = INPUT_KEYBOARD_OFFSET + 198;
const InputCode INPUT_SCANCODE_KP_AMPERSAND = INPUT_KEYBOARD_OFFSET + 199;
const InputCode INPUT_SCANCODE_KP_DBLAMPERSAND = INPUT_KEYBOARD_OFFSET + 200;
const InputCode INPUT_SCANCODE_KP_VERTICALBAR = INPUT_KEYBOARD_OFFSET + 201;
const InputCode INPUT_SCANCODE_KP_DBLVERTICALBAR = INPUT_KEYBOARD_OFFSET + 202;
const InputCode INPUT_SCANCODE_KP_COLON = INPUT_KEYBOARD_OFFSET + 203;
const InputCode INPUT_SCANCODE_KP_HASH = INPUT_KEYBOARD_OFFSET + 204;
const InputCode INPUT_SCANCODE_KP_SPACE = INPUT_KEYBOARD_OFFSET + 205;
const InputCode INPUT_SCANCODE_KP_AT = INPUT_KEYBOARD_OFFSET + 206;
const InputCode INPUT_SCANCODE_KP_EXCLAM = INPUT_KEYBOARD_OFFSET + 207;
const InputCode INPUT_SCANCODE_KP_MEMSTORE = INPUT_KEYBOARD_OFFSET + 208;
const InputCode INPUT_SCANCODE_KP_MEMRECALL = INPUT_KEYBOARD_OFFSET + 209;
const InputCode INPUT_SCANCODE_KP_MEMCLEAR = INPUT_KEYBOARD_OFFSET + 210;
const InputCode INPUT_SCANCODE_KP_MEMADD = INPUT_KEYBOARD_OFFSET + 211;
const InputCode INPUT_SCANCODE_KP_MEMSUBTRACT = INPUT_KEYBOARD_OFFSET + 212;
const InputCode INPUT_SCANCODE_KP_MEMMULTIPLY = INPUT_KEYBOARD_OFFSET + 213;
const InputCode INPUT_SCANCODE_KP_MEMDIVIDE = INPUT_KEYBOARD_OFFSET + 214;
const InputCode INPUT_SCANCODE_KP_PLUSMINUS = INPUT_KEYBOARD_OFFSET + 215;
const InputCode INPUT_SCANCODE_KP_CLEAR = INPUT_KEYBOARD_OFFSET + 216;
const InputCode INPUT_SCANCODE_KP_CLEARENTRY = INPUT_KEYBOARD_OFFSET + 217;
const InputCode INPUT_SCANCODE_KP_BINARY = INPUT_KEYBOARD_OFFSET + 218;
const InputCode INPUT_SCANCODE_KP_OCTAL = INPUT_KEYBOARD_OFFSET + 219;
const InputCode INPUT_SCANCODE_KP_DECIMAL = INPUT_KEYBOARD_OFFSET + 220;
const InputCode INPUT_SCANCODE_KP_HEXADECIMAL = INPUT_KEYBOARD_OFFSET + 221;
const InputCode INPUT_SCANCODE_LCTRL = INPUT_KEYBOARD_OFFSET + 224;
const InputCode INPUT_SCANCODE_LSHIFT = INPUT_KEYBOARD_OFFSET + 225;
const InputCode INPUT_SCANCODE_LALT = INPUT_KEYBOARD_OFFSET + 226;
const InputCode INPUT_SCANCODE_LGUI = INPUT_KEYBOARD_OFFSET + 227;
const InputCode INPUT_SCANCODE_RCTRL = INPUT_KEYBOARD_OFFSET + 228;
const InputCode INPUT_SCANCODE_RSHIFT = INPUT_KEYBOARD_OFFSET + 229;
const InputCode INPUT_SCANCODE_RALT = INPUT_KEYBOARD_OFFSET + 230;
const InputCode INPUT_SCANCODE_RGUI = INPUT_KEYBOARD_OFFSET + 231;
const InputCode INPUT_SCANCODE_MODE = INPUT_KEYBOARD_OFFSET + 257;
const InputCode INPUT_SCANCODE_AUDIONEXT = INPUT_KEYBOARD_OFFSET + 258;
const InputCode INPUT_SCANCODE_AUDIOPREV = INPUT_KEYBOARD_OFFSET + 259;
const InputCode INPUT_SCANCODE_AUDIOSTOP = INPUT_KEYBOARD_OFFSET + 260;
const InputCode INPUT_SCANCODE_AUDIOPLAY = INPUT_KEYBOARD_OFFSET + 261;
const InputCode INPUT_SCANCODE_AUDIOMUTE = INPUT_KEYBOARD_OFFSET + 262;
const InputCode INPUT_SCANCODE_MEDIASELECT = INPUT_KEYBOARD_OFFSET + 263;
const InputCode INPUT_SCANCODE_WWW = INPUT_KEYBOARD_OFFSET + 264;
const InputCode INPUT_SCANCODE_MAIL = INPUT_KEYBOARD_OFFSET + 265;
const InputCode INPUT_SCANCODE_CALCULATOR = INPUT_KEYBOARD_OFFSET + 266;
const InputCode INPUT_SCANCODE_COMPUTER = INPUT_KEYBOARD_OFFSET + 267;
const InputCode INPUT_SCANCODE_AC_SEARCH = INPUT_KEYBOARD_OFFSET + 268;
const InputCode INPUT_SCANCODE_AC_HOME = INPUT_KEYBOARD_OFFSET + 269;
const InputCode INPUT_SCANCODE_AC_BACK = INPUT_KEYBOARD_OFFSET + 270;
const InputCode INPUT_SCANCODE_AC_FORWARD = INPUT_KEYBOARD_OFFSET + 271;
const InputCode INPUT_SCANCODE_AC_STOP = INPUT_KEYBOARD_OFFSET + 272;
const InputCode INPUT_SCANCODE_AC_REFRESH = INPUT_KEYBOARD_OFFSET + 273;
const InputCode INPUT_SCANCODE_AC_BOOKMARKS = INPUT_KEYBOARD_OFFSET + 274;
const InputCode INPUT_SCANCODE_BRIGHTNESSDOWN = INPUT_KEYBOARD_OFFSET + 275;
const InputCode INPUT_SCANCODE_BRIGHTNESSUP = INPUT_KEYBOARD_OFFSET + 276;
const InputCode INPUT_SCANCODE_DISPLAYSWITCH = INPUT_KEYBOARD_OFFSET + 277;
const InputCode INPUT_SCANCODE_KBDILLUMTOGGLE = INPUT_KEYBOARD_OFFSET + 278;
const InputCode INPUT_SCANCODE_KBDILLUMDOWN = INPUT_KEYBOARD_OFFSET + 279;
const InputCode INPUT_SCANCODE_KBDILLUMUP = INPUT_KEYBOARD_OFFSET + 280;
const InputCode INPUT_SCANCODE_EJECT = INPUT_KEYBOARD_OFFSET + 281;
const InputCode INPUT_SCANCODE_SLEEP = INPUT_KEYBOARD_OFFSET + 282;
const InputCode INPUT_SCANCODE_APP1 = INPUT_KEYBOARD_OFFSET + 283;
const InputCode INPUT_SCANCODE_APP2 = INPUT_KEYBOARD_OFFSET + 284;
const InputCode INPUT_SCANCODE_AUDIOREWIND = INPUT_KEYBOARD_OFFSET + 285;
const InputCode INPUT_SCANCODE_AUDIOFASTFORWARD = INPUT_KEYBOARD_OFFSET + 286;
const InputCode INPUT_SCANCODE_SOFTLEFT = INPUT_KEYBOARD_OFFSET + 287;
const InputCode INPUT_SCANCODE_SOFTRIGHT = INPUT_KEYBOARD_OFFSET + 288;
const InputCode INPUT_SCANCODE_CALL = INPUT_KEYBOARD_OFFSET + 289;
const InputCode INPUT_SCANCODE_ENDCALL = INPUT_KEYBOARD_OFFSET + 290;

// Mouse input
// Base from SDL_mouse.h
// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_mouse.h#L462
const InputCode INPUT_MOUSE_BUTTON_LEFT = INPUT_MOUSE_OFFSET + 1;
const InputCode INPUT_MOUSE_BUTTON_MIDDLE = INPUT_MOUSE_OFFSET + 2;
const InputCode INPUT_MOUSE_BUTTON_RIGHT = INPUT_MOUSE_OFFSET + 3;
const InputCode INPUT_MOUSE_BUTTON_X1 = INPUT_MOUSE_OFFSET + 4;
const InputCode INPUT_MOUSE_BUTTON_X2 = INPUT_MOUSE_OFFSET + 5;

// Controller 1 input
// Base from SDL_gamepad.h
// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_gamepad.h#L85C1-L106C1
const InputCode CONTROLLER1_BUTTON_A = INPUT_CONTROLLER1_OFFSET + 1;
const InputCode CONTROLLER1_BUTTON_B = INPUT_CONTROLLER1_OFFSET + 2;
const InputCode CONTROLLER1_BUTTON_X = INPUT_CONTROLLER1_OFFSET + 3;
const InputCode CONTROLLER1_BUTTON_Y = INPUT_CONTROLLER1_OFFSET + 4;
const InputCode CONTROLLER1_BUTTON_BACK = INPUT_CONTROLLER1_OFFSET + 5;
const InputCode CONTROLLER1_BUTTON_GUIDE = INPUT_CONTROLLER1_OFFSET + 6;
const InputCode CONTROLLER1_BUTTON_START = INPUT_CONTROLLER1_OFFSET + 7;
const InputCode CONTROLLER1_BUTTON_LEFT_STICK = INPUT_CONTROLLER1_OFFSET + 8;
const InputCode CONTROLLER1_BUTTON_RIGHT_STICK = INPUT_CONTROLLER1_OFFSET + 9;
const InputCode CONTROLLER1_BUTTON_LEFT_SHOULDER = INPUT_CONTROLLER1_OFFSET + 10;
const InputCode CONTROLLER1_BUTTON_RIGHT_SHOULDER = INPUT_CONTROLLER1_OFFSET + 11;
const InputCode CONTROLLER1_BUTTON_DPAD_UP = INPUT_CONTROLLER1_OFFSET + 12;
const InputCode CONTROLLER1_BUTTON_DPAD_DOWN = INPUT_CONTROLLER1_OFFSET + 13;
const InputCode CONTROLLER1_BUTTON_DPAD_LEFT = INPUT_CONTROLLER1_OFFSET + 14;
const InputCode CONTROLLER1_BUTTON_DPAD_RIGHT = INPUT_CONTROLLER1_OFFSET + 15;
const InputCode CONTROLLER1_BUTTON_MISC1 = INPUT_CONTROLLER1_OFFSET + 16;    // Xbox Series X share button PS5 microphone button Nintendo Switch Pro capture button Amazon Luna microphone button
const InputCode CONTROLLER1_BUTTON_PADDLE1 = INPUT_CONTROLLER1_OFFSET + 17;  // Xbox Elite paddle P1 (upper left facing the back)
const InputCode CONTROLLER1_BUTTON_PADDLE2 = INPUT_CONTROLLER1_OFFSET + 18;  // Xbox Elite paddle P3 (upper right facing the back)
const InputCode CONTROLLER1_BUTTON_PADDLE3 = INPUT_CONTROLLER1_OFFSET + 19;  // Xbox Elite paddle P2 (lower left facing the back)
const InputCode CONTROLLER1_BUTTON_PADDLE4 = INPUT_CONTROLLER1_OFFSET + 20;  // Xbox Elite paddle P4 (lower right facing the back)
const InputCode CONTROLLER1_BUTTON_TOUCHPAD = INPUT_CONTROLLER1_OFFSET + 21; // PS4/PS5 touchpad button

const InputCode INPUT_LAST = CONTROLLER1_BUTTON_TOUCHPAD + 1;

// Controller axis
// Based from SDL_gamepad.h
// https://github.com/libsdl-org/SDL/blob/main/include/SDL3/SDL_gamepad.h#L121
const InputCode CONTROLLER_AXIS_LEFTX = 0;
const InputCode CONTROLLER_AXIS_LEFTY = 1;
const InputCode CONTROLLER_AXIS_RIGHTX = 2;
const InputCode CONTROLLER_AXIS_RIGHTY = 3;
const InputCode CONTROLLER_AXIS_LEFT_TRIGGER = 4;
const InputCode CONTROLLER_AXIS_RIGHT_TRIGGER = 5;

enum class InputSource {
    Unknown = 1,
    Keyboard,
    Mouse,
    Controller,
};

enum class InputAction {
    Unknown = 1,
    Pressed,
    Released,
    Moved,
};

struct InputEvent {
    InputSource source = InputSource::Unknown;
    InputAction action = InputAction::Unknown;
    InputCode code = INPUT_CODE_NONE;
};

//
//
//enum class ActionCodeSlot {
//    Slot1,
//    Slot2,
//};
//
//struct Input {
//    NomadId id;
//    NomadString name;
//    // Code of input, depending on the source:
//    // - SDL_Scancode + INPUT_KEY_OFFSET for keyboard
//    // - SDL_MouseButton + INPUT_MOUSE_OFFSET for mouse
//    // - SDL_GameControllerButton + INPUT_CONTROLLER1_OFFSET for controller
//    InputCode code1 = INPUT_CODE_NONE;  // code for the first slot
//    InputCode code2 = INPUT_CODE_NONE;  // code for the second slot
//    bool pressed = false;  // true if the input was pressed this frame
//    bool released = false;  // true if the input was released this frame
//    bool held = false;  // true if the input is currently held
//};

NomadString get_input_code_name(InputCode code);
InputCode get_input_code(const NomadString& name);

void map_sdl_keyboard_event(SDL_KeyboardEvent& event, InputEvent& input_event);
void map_sdl_mouse_event(SDL_MouseButtonEvent& event, InputEvent& input_event);
void map_sdl_controller_button_event(SDL_ControllerButtonEvent& event, InputEvent& input_event);
void map_sdl_controller_axis_event(SDL_ControllerAxisEvent& event, InputEvent& input_event);

//class InputManager {
//public:
//    InputManager(Game* game);
//    InputManager(const InputManager&) = delete;
//    ~InputManager() = default;
//
//    Game* get_game();
//
//    void on_start_frame();
//    void on_end_frame();
//
//    void save_mapping(const NomadString& mapping_name);
//    void load_mapping(const NomadString& mapping_name);
//
//    void clear_all_actions();
//    NomadId register_action(const NomadString& name, InputCode code1 = INPUT_CODE_NONE, InputCode code2 = INPUT_CODE_NONE);
//
//    void set_code_for_action(const NomadString& action_name, ActionCodeSlot slot, InputCode input_code);
//
//    void update_input(SDL_KeyboardEvent& event);
//    void update_input(SDL_MouseButtonEvent& event);
//    void update_input(SDL_ControllerButtonEvent& event);
//    [[nodiscard]] NomadId get_input_id(const NomadString& name) const;
//    [[nodiscard]] const Input& get_input(NomadId id) const;
//
//    [[nodiscard]] bool is_pressed(NomadId id) const;
//    [[nodiscard]] bool is_released(NomadId id) const;
//    [[nodiscard]] bool is_held(NomadId id) const;
//
//    [[nodiscard]] bool is_pressed(const NomadString& action_name) const;
//    [[nodiscard]] bool is_released(const NomadString& action_name) const;
//    [[nodiscard]] bool is_held(const NomadString& action_name) const;
//
//private:
//    Game* m_game;
//    std::vector<Input> m_inputs;
//};

} // nomad

#endif //NOMAD_INPUTMANAGER_HPP
