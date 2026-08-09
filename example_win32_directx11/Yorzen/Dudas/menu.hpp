#pragma once

// Shared active tab index (C++17 inline — one symbol across TUs).
inline int iTabs = 0;

// Renders all main menu tab panels (Aim / Visual / Brutal / Keybinds / Settings).
void YorzenRenderMenuTabs(float fTabOffset);
