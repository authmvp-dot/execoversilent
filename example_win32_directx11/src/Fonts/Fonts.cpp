#include "Fonts.hpp"
#include "FontAwesome.hpp"
#include "FontInter.hpp"
#include "icon.h"
#include "FontsBytes.h"

namespace FWork {
    static void BuildEspNameGlyphRanges(ImGuiIO& io, ImVector<ImWchar>& ranges) {
        ImFontGlyphRangesBuilder builder;
        builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
        builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
        builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
        builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
        builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
        builder.AddText((const char*)u8"\n"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n\n"
            "✓ ✔ ✗ ✘ ☓ χ ✕ ❎ ❌ ˣ ₓ 〤∨ √ ✔️ ✅🆅 🅥 ☐ ☑ ☒ 🆇 🅧 ⊗ ⨷〥\n\n"
            "↕ ↖ ↗ ↘ ↙ ↚ ↛ ↜ ↝ ↞ ↟ ↠ ↡ ↢ ↣ ↤ ↥ ↦ ↧ ↨ ↩ ↪ ↫ ↬ ↭ ↮ ↯ ↰ ↱ ↲ ↳ ↴ ↶ ↷ ↸ ↹ ↺ ↻ ↼ ↽ ↾ ↿ ⇀ ⇁ ⇂ ⇃ ⇄ ⇅ ⇆ ⇇ ⇈ ⇉ ⇊ ⇋ ⇌ ⇍ ⇎ ⇏ ⇕ ⇖ ⇗ ⇘ ⇙ ⇚ ⇛ ⇜ ⇝ ⇞ ⇟ ⇠ ⇡ ⇢ ⇣ ⇤ ⇥ ⇦ ⇧ ⇨ ⇩ ⇪ ⌅ ⌆ ⌤ ⏎ ▶ ☇ ☈ ☊ ☋ ☌ ☍ ➔ ➘ ➙ ➚ ➛ ➜ ➝ ➞ ➟ ➠ ➡ ➢ ➣ ➤ ➥ ➦ ➧ ➨ ➩ ➪ ➫ ➬ ➭ ➮ ➯ ➱ ➲ ➳ ➴ ➵ ➶ ➷ ➸ ➹ ➺ ➻ ➼ ➽ ➾ ⤴ ⤵ ↵ ↓ ↔ ← → ↑ ⌦ ⌫ ⌧ ⇰ ⇫ ⇬ ⇭ ⇳ ⇮ ⇯ ⇱ ⇲ ⇴ ⇵ ⇷ ⇸ ⇹ ⇺ ⇑ ⇓ ⇽ ⇾ ⇿ ⬳ ⟿ ⤉ ⤈ ⇻ ⇼ ⬴ ⤀ ⬵ ⤁ ⬹ ⤔ ⬺ ⤕ ⬶ ⤅ ⬻ ⤖ ⬷ ⤐ ⬼ ⤗ ⬽ ⤘ ⤝ ⤞ ⤟ ⤠ ⤡ ⤢ ⤣ ⤥ ⤦ ⤪ ⤨ ⤧ ⤩ ⤭ ⤮ ⤯ ⤰ ⤱ ⤲ ⤫ ⤬ ⬐ ⬎ ⬑ ⬏ ⤶ ⤷ ⥂ ⥃ ⥄ ⭀ ⥱ ⥶ ⥸ ⭂ ⭈ ⭊ ⥵ ⭁ ⭇ ⭉ ⥲ ⭋ ⭌ ⥳ ⥴ ⥆ ⥅ ⥹ ⥻ ⬰ ⥈ ⬾ ⥇ ⬲ ⟴ ⥷ ⭃ ⥺ ⭄ ⥉ ⥰ ⬿ ⤳ ⥊ ⥋ ⥌ ⥍ ⥎ ⥏ ⥐ ⥑ ⥒ ⥓ ⥔ ⥕ ⥖ ⥗ ⥘ ⥙ ⥚ ⥛ ⥜ ⥝ ⥞ ⥟ ⥠ ⥡ ⥢ ⥤ ⥣ ⥥ ⥦ ⥨ ⥧ ⥩ ⥮ ⥯ ⥪ ⥬ ⥫ ⥭ ⤌ ⤍ ⤎ ⤏ ⬸ ⤑ ⬱ ⟸ ⟹ ⟺ ⤂ ⤃ ⤄ ⤆ ⤇ ⤊ ⤋ ⭅ ⭆ ⟰ ⟱ ⇐ ⇒ ⇔ ⇶ ⟵ ⟶ ⟷ ⬄ ⬀ ⬁ ⬂ ⬃ ⬅ ⬆ ⬇ ⬈ ⬉ ⬊ ⬋ ⬌ ⬍ ⟻ ⟼ ⤒ ⤓ ⤙ ⤚ ⤛ ⤜ ⥼ ⥽ ⥾ ⥿ ⤼ ⤽ ⤾ ⤿ ⤸ ⤺ ⤹ ⤻ ⥀ ⥁ ⟲ ⟳\n\n"
            "ÁÉÍÓÚÜÑÇàèìòùüñçÅåÄäÖö\n\n"
            "★ ☆ ✡ ✦ ✧ ✩ ✪ ✫ ✬ ✭ ✮ ✯ ✰ ⁂ ⁎ ⁑ ✢ ✣ ✤ ✥ ✱ ✲ ✳ ✴ ✵ ✶ ✷ ✸ ✹ ✺ ✻ ✼ ✽ ✾ ✿ ❀ ❁ ❂ ❃ ❇ ❈ ❉ ❊ ❋ ❄ ❆ ❅ ⋆ ≛ ᕯ ✲ ࿏ ꙰ ۞ ⭒ ⍟ ⭐ 🌠 🌟 💫 ✨ 🌃 🔯 ❁  ⚜  ⚘ ⁕ ꙮ ꕤ ꕥ ☘ ֍ ֎ ☠ ☤ ☥ ☦ ☧ ☨ ☩ ☪ ☫ ☬ ☮ ☭ ☯ ☸ ☽ ☾ ♕ ♚ ♛ ✙ ✚ ✛ ✜ ✝ ✞ ✟ ✠ ✡ ✢ † ☓ ♁ ♆\n\n"
            "ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩαβγδεζηθικλμνξοπρςστυφχψω\n\n"
            "° ℃ ℉ ϟ ☀ ☁ ☂ ☃ ☉ ☼ ☽ ☾ ♁ ♨ ❄ ❅ ❆ ☇ ☈ ☄ ༄ ࿓ ㎎ ㎏ ㎜ ㎝ ㎞ ㎡ ㏄ ㏎ ㏑ ㏒ ㏕ ♁ ♨ ❄ ❅ ❆ ༄ ✺ ☇ ☈ ★ ☆ ℃ ℉ °\n\n"
            "1234567890\n"
            "①②③④⑤⑥⑦⑧⑨⓪\n"
            "➀➁➂➃➄➅➆➇➈➉\n\n"
            "∀∃∅∈∉⊂⊃∪∩≤≥≠≈≡∫∞∂√∇\n"
            "±×÷¼½¾ªº$€£¥¢‰§¶\n\n"
            "♠♣♥♦★☆☀☂☃☎☏☑☒☢☣☮☯♻⚽⚾\n"
            "⚔⚖⚗⚙⚛⚜⚡⚪⚫⛄⛅⛈⛎⛔❤️\n"
            "⌛⏳⌚⏰⏱⏲⏸⏹⏺⏭⏮⏩⏪⏫⏬\n\n"
            "😀😁😂🤣😃😄😅😆😉😊😋😎\n\n"
            "ᴀʙᴄᴅᴇꜰɢʜɪᴊᴋʟᴍɴᴏᴘQʀꜱᴛᴜᴠᴡxʏᴢ\n"
            "ᵃᵇᶜᵈᵉᶠᵍʰⁱʲᵏˡᵐⁿᵒᵖqʳˢᵗᵘᵛʷˣʸᶻ\n"
            "ǟɮƈɖɛʄɢɦɨʝӄʟʍռօքզʀֆȶʊʋաӼʏʐ\n"
            "A҉B҉C҉D҉E҉F҉G҉H҉I҉J҉K҉L҉M҉N҉O҉P҉Q҉R҉S҉T҉U҉V҉W҉X҉Y҉Z҉\n"
            "ᗩᗷᑕᗪEᖴGᕼIᒍKᒪᗰᑎOᑭᑫᖇᔕTᑌᐯᗯ᙭Yᘔ\n"
            "₳฿₵ĐɆ₣₲ⱧłJ₭Ⱡ₥₦Ø₱QⱤ₴₮ɄV₩ӾɎⱫ\n"
            "ⒶⒷⒸⒹⒺⒻⒼⒽⒾⒿⓀⓁⓂⓃⓄⓅⓆⓇⓈⓉⓊⓋⓌⓍⓎⓏ\n"
            "ⓐⓑⓒⓓⓔⓕⓖⓘⓙⓚⓛⓜⓝⓞⓟⓠⓡⓢⓣⓤⓥⓦⓧⓨⓩ\n\n"
            "卂乃匚ᗪ乇千Ꮆ卄丨ﾌҜㄥ爪几ㄖ卩Ɋ尺丂ㄒㄩᐯ山乂ㄚ乙\n"
            "ꍏꌃꉓꀸꍟꎇꁅꃅꀤꀭꀘꎭꈤ꒒ꂦᖘꆰꋪꌗ꓄ꀎᐯꅏꊼꌩꁴ\n"
            "愛ᴱˢᴾᴬᴺᶜᴬ BCT\n"
            "這還♤♤__ぁ¡¿«»©®™°\n\n"
            "K7ﾠʜᴀᴠᴀɪᴀɴᴀs\n"
            "ㅤСDGᴄʟᴋㅤВL4CKㅤA, Promesinㅤсdg, SESㅤPS, , GAMORA DO GF™SOCAﾠFOFO✓ᵘᵐᵖκɪɴɢ.あ{ Kauã }\"Name\"\n"
            "ঔৣ☬✞𝕿𝕮𝕷𝕬𝕹☬ঔৣ꧂BABY FF ⇄ ღ²¹ˢⱠɄ₵ŀ₣ɆⱤ❅♨.()#➳ᴹᴿ᭄Đ₳Vł✤\n"
            "ﾠ⚡┋꧁꧂☯ᵐᵖ⁴⁰¹⁵⁷₁₅₇ⁿᵗʲʰˢᶠᵉᴹᴿ°᭄Sᴋ᭄ᴮᵒˢˢᵝᵒˢˢAᴋ᭄ᴬᴷ᭄༄ᶦᶰᵈ᭄ᴾᴿᴼ ᭄＠ᵞᵗ ᭄﹫ⓐ♈♉♊♋♌♍♎♏♐♑♒®♓♨ﮩ٨ـﮩﮩ٨ـ♡ﮩ٨ـﮩﮩ٨ـﮩ٨ـﮩﮩ٨ـ⌛☬©☕♬ﮩ٨ـﮩﮩ٨ـ♥ﮩ٨ـﮩﮩ٨ـᤢㅤ\n"
            "ꪰꪰ⛺炎ぁア$ホ愛✅ⓥⓋᯤ⁶⁹ᯤ⁹⁹⁹ᯤ⁴⁴⁴ᯤ⳻͟⳺⳻᷼⳺望™ꛍ♂️〆么テ©️✓彡ꪜஐღლ۵დ✠✙✥✛✞✟༒❀✿ꙮ✽⁕ꕤꕥ＊❄᪥⛄☃ᖽ⚡Ϟϟ⁂☀☂☔☁⛅❤★♱╰☆╮☆★⭐*⁎⁑꙳﹡ㅤ҉｡･♠♥♦♣♤♡♢♧♲♻ʚїɞᴥⰇ߷⁶⁹Ⓥⓥ🅥🆅ⓥ🅅ㅤ✿ ™ ©❤★♫∞♞♛☀☂☎☏☺♔♕♖♗♘♙♚♛♜♝♞♟⚀⚁⚂⚃⚄⚅☘☢☣☤☥☦☧☨☩☪☫☬☭☮☯☸☹☺☻☼☽☾☿♀♁♂♃♄♅♆♇♈♉♊♋♌♍♎♏♐♑♒♓♔♕♖♗♘♙♚♛♜♝♞♟♠♡♢♣♤♥♦♧♨♩♪♫♬♭♮♯♰♱✁✂✃✄✆✇✈✉✌✍✎✏✐✑✒✓✔✕✖✗✘✙✚✛✜✝✞✟✠✡✢✣✤✥✦✧✩✪✫✬✭✮✯✰\n"
            " ⁴⁴⁴ᴮᵃᙆᓑᵏᵃᵇᵃˡᵃ!✿ᔆᵃᵈ⚡₁₅₇ﾂ.ﾠʳʲ⽀ᶠᵘᶜᵏﾠᘁᵉʳᵐᵉ⸸ᶠᵉ♱㍶ﾠďﾠﾠ༒ᴳᴬӲˣᵒᵗᵃᵈᵒᶜᵉ愛ⁿᵗʲˣᵖ †ᴘᴀᴛʀᴀᴏﾠ〆ғғᵐᵖ⁴⁰愛♀️ꓟô ❤☯ﮩᴾˡᵃʸᵉʳᵒᶠᶠ!™ᶜᵃᵗ¹⁵⁷ﾠᶠˡᵉˣᴾˡᵃʸᵒᵌⁿᵒˢᴰᵉᴵᵃˢ!ғʀᴏѕт.愛 ☂ぁ⛺ᒾ¹. ꚠ!ѕtєɪkє ぁ⛺ᒾ¹♿Ｃｈｅｆｉｎ⚡₁₅₇I'amㅤclostㅤ〆╰︵╯ ꞤȺȾȾɎ🤬BOLADO👿ᴮᴿ    red   ☁ Ｂｌｅｓｓｅｄ 愛ˣᣴᶜ🤣½⅓⅓⅕⅛☮☸☪☉☽☫☬☭☯✝✞✟†♆❖✠❖卍卐☁♨☠☢\n"
        );
        builder.BuildRanges(&ranges);
    }

    void Fonts::LoadEspNameFonts() {
        if (NotoSansRegular)
            return;

        ImGuiIO& io = ImGui::GetIO();
        ImFontConfig unicodeConfig;
        unicodeConfig.OversampleH = 3;
        unicodeConfig.OversampleV = 3;
        ImVector<ImWchar> ranges;
        BuildEspNameGlyphRanges(io, ranges);

        NotoSansRegular = io.Fonts->AddFontFromMemoryCompressedTTF(NotoSansRegular_compressed_data, NotoSansRegular_compressed_size, 18.0f, &unicodeConfig, ranges.Data);
        unicodeConfig.MergeMode = true;
        unicodeConfig.PixelSnapH = true;
        NotoSansSymbolsRegular = io.Fonts->AddFontFromMemoryCompressedTTF(NotoSansSymbolsRegular_compressed_data, NotoSansSymbolsRegular_compressed_size, 18.0f, &unicodeConfig, ranges.Data);
        ImFontConfig emoji_config;
        emoji_config.MergeMode = true;
        emoji_config.PixelSnapH = true;
        emoji_config.GlyphOffset.y = 2.0f;
        NotoEmojiRegular = io.Fonts->AddFontFromMemoryCompressedTTF(NotoEmojiRegular_compressed_data, NotoEmojiRegular_compressed_size, 18.0f, &emoji_config, ranges.Data);
        ArialUnicode = io.Fonts->AddFontFromMemoryCompressedTTF(arialunicodems_compressed_data, arialunicodems_compressed_size, 18.0f, &emoji_config, ranges.Data);
        LexendRegular = io.Fonts->AddFontFromMemoryCompressedTTF(LexendRegular_compressed_data, LexendRegular_compressed_size, 18.0f, &emoji_config, ranges.Data);

        io.Fonts->Build();
    }

    void Fonts::Initialize(ID3D11Device* Device) {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();
        NotoSansRegular = nullptr;
        NotoSansSymbolsRegular = nullptr;
        NotoEmojiRegular = nullptr;
        ArialUnicode = nullptr;
        LexendRegular = nullptr;

        // Fonts Menu
        ImFontConfig FontAwesomeConfig;
        FontAwesomeConfig.GlyphMinAdvanceX = 25.f * (2.0f / 3.0f);
        static const ImWchar IconRanges[] = {
            ICON_MIN_FA, ICON_MAX_FA, 0
        };
        InterBlack = io.Fonts->AddFontFromMemoryCompressedTTF(InterBlack_compressed_data, InterBlack_compressed_size, 14);
        InterBold = io.Fonts->AddFontFromMemoryCompressedTTF(InterBold_compressed_data, InterBold_compressed_size, 17);
        InterBold12 = io.Fonts->AddFontFromMemoryCompressedTTF(InterBold_compressed_data, InterBold_compressed_size, 15);
        InterExtraBold = io.Fonts->AddFontFromMemoryCompressedTTF(InterExtraBold_compressed_data, InterExtraBold_compressed_size, 13);
        InterExtraLight = io.Fonts->AddFontFromMemoryCompressedTTF(InterExtraLight_compressed_data, InterExtraLight_compressed_size, 14);
        InterLight = io.Fonts->AddFontFromMemoryCompressedTTF(InterLight_compressed_data, InterLight_compressed_size, 12);
        InterMedium = io.Fonts->AddFontFromMemoryCompressedTTF(InterMedium_compressed_data, InterMedium_compressed_size, 17);
        InterRegular = io.Fonts->AddFontFromMemoryCompressedTTF(InterRegular_compressed_data, InterRegular_compressed_size, 17);
        InterRegular14 = io.Fonts->AddFontFromMemoryCompressedTTF(InterRegular_compressed_data, InterRegular_compressed_size, 15);
        InterSemiBold = io.Fonts->AddFontFromMemoryCompressedTTF(InterSemiBold_compressed_data, InterSemiBold_compressed_size, 16);
        InterThin = io.Fonts->AddFontFromMemoryCompressedTTF(InterThin_compressed_data, InterThin_compressed_size, 14);

        LoadEspNameFonts();

        // Fonts Weapon
       

        // Fonts Icons Menu
        FontAwesomeRegular = io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesomeRegular_compressed_data, FontAwesomeRegular_compressed_size, 25.f * (2.0f / 3.0f), &FontAwesomeConfig, &IconRanges[0]);
        FontAwesomeSolid = io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesomeSolid_compressed_data, FontAwesomeSolid_compressed_size, 27.f * (2.0f / 3.0f), &FontAwesomeConfig, &IconRanges[0]);
        FontAwesomeSolid18 = io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesomeSolid_compressed_data, FontAwesomeSolid_compressed_size, 18.f * (2.0f / 3.0f), &FontAwesomeConfig, &IconRanges[0]);
        FontAwesomeSolidBig = io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesomeSolid_compressed_data, FontAwesomeSolid_compressed_size, 30.f * (2.0f / 3.0f), &FontAwesomeConfig, &IconRanges[0]);

        // Imagem
        //D3DX11CreateShaderResourceViewFromMemory(Device, RawBytes, sizeof(RawBytes), NULL, NULL, &Logo, NULL);

        io.Fonts->Build();
    }
}
