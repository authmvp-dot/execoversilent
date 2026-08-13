using BLACKSKYMEM;
using Client;
using Guna.UI2.WinForms;
using Helper;
using Keyauth;
using Memory;
using Microsoft.VisualBasic.ApplicationServices;
using Microsoft.Win32;
using NARZOPAPA;
using Newtonsoft.Json.Linq;
using SixLabors.ImageSharp.ColorSpaces;
using System.Diagnostics;
using System.Diagnostics.Eventing.Reader;
using System.Management;
using System.Media;
using System.Net;
using System.Net.Sockets;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Text;
using static AotForms.ESP;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.StartPanel;

namespace AotForms
{
    public partial class MainMenu : Form
    {



        bool isLoggedIn = false;
        bool connected = false;
        bool startk = false;
        bool initSuccess = false;

        private bool EnsureLoggedIn()
        {
            if (isLoggedIn)
                return true;

            info.Show("Please login first");
            return false;
        }

        private bool EnsureInjected()
        {
            if (!EnsureLoggedIn())
                return false;

            if (connected)
                return true;

            info.Show("Restart lobby then inject");
            return false;
        }


        private System.Windows.Forms.Timer timer = new System.Windows.Forms.Timer();




        private const int WM_HOTKEY = 0x0312;
        private const int WM_KEYDOWN = 0x0100;
        private const int WM_SYSKEYDOWN = 0x0104;
        private const int INSERT_HOTKEY_ID = 1;

        private readonly Dictionary<int, int> _vkToHotkeyIndex = new();
        private bool _isBindingKey;
        private int _lastHotkeyVk = -1;
        private long _lastHotkeyTick;

        // ================= STREAMER MODE API =================
        [DllImport("user32.dll")]
        private static extern bool SetWindowDisplayAffinity(IntPtr hWnd, uint dwAffinity);

        private const uint WDA_NONE = 0x0;
        private const uint WDA_EXCLUDEFROMCAPTURE = 0x11;

        // ================= HOTKEY REGISTER API =================
        [DllImport("user32.dll")]
        private static extern bool RegisterHotKey(IntPtr hWnd, int id, uint fsModifiers, uint vk);

        [DllImport("user32.dll")]
        private static extern bool UnregisterHotKey(IntPtr hWnd, int id);


        // ================= STREAMER & HOTKEY CORE =================

        private Keys[] boundKeys = new Keys[13];


        [DllImport("user32.dll")]
        static extern int GetWindowLong(IntPtr hWnd, int nIndex);

        [DllImport("user32.dll")]
        static extern int SetWindowLong(IntPtr hWnd, int nIndex, int dwNewLong);

        private string configFilePath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.Desktop), "config.txt");
        private LowLevelKeyboardProc _proc;
        private IntPtr _hookID = IntPtr.Zero;





        IntPtr mainHandle;
        KeyHelper kh = new KeyHelper();
        private CX memoryfast = new CX();

        public static MainMenu Instance { get; private set; }

        public MainMenu(IntPtr handle)
        {
            Instance = this;
            mainHandle = handle;
            InitializeComponent();

            try
            {
                KeyAuthApp.init();

                if (!KeyAuthApp.response.success)
                {
                    // init() returned but response is not success — could be
                    // a version mismatch, banned HWID, or server-side issue.
                    // NOTE: KeyAuth's init() doesn't always populate response.success
                    // on first init; the sessionid being set is the real indicator.
                    // We treat it as success if no exception was thrown.
                    Debug.WriteLine("[KeyAuth] init response.success=false, msg: " + KeyAuthApp.response.message);
                }

                initSuccess = true;
                Debug.WriteLine("[KeyAuth] Initialization successful.");
            }
            catch (Exception ex)
            {
                initSuccess = false;
                Debug.WriteLine("[KeyAuth] Init EXCEPTION: " + ex.Message);
                MessageBox.Show(
                    "KeyAuth Init Failed:\n" + ex.Message,
                    "Initialization Error",
                    MessageBoxButtons.OK,
                    MessageBoxIcon.Error
                );
            }

            LoadSettings();
            UpdateUserUI();
            this.KeyPreview = true;
            guna2DragControl18.TargetControl = guna2Panel1;
            _proc = HookCallback;
            _hookID = SetHook(_proc);
            this.FormClosing += MainMenu_FormClosing;
            this.KeyPreview = true;

        }

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
            RegisterHotKey(this.Handle, INSERT_HOTKEY_ID, 0, (uint)Keys.Insert);
        }

        // ================= BIND KEY =====================
        private void BindKey(int index, Guna.UI2.WinForms.Guna2Button button)
        {
            _isBindingKey = true;
            button.Text = "Press";
            Focus();
            KeyPreview = true;

            KeyEventHandler handler = null;
            handler = (sender, e) =>
            {
                if (e.KeyCode == Keys.Escape)
                {
                    button.Text = "-";
                    _isBindingKey = false;
                    KeyDown -= handler;
                    return;
                }

                ClearHotkeyBinding(index);

                foreach (var kvp in _vkToHotkeyIndex.ToList())
                {
                    if (kvp.Key == (int)e.KeyCode && kvp.Value != index)
                    {
                        ClearHotkeyBinding(kvp.Value);
                        GetHotkeyButton(kvp.Value).Text = "-";
                    }
                }

                boundKeys[index] = e.KeyCode;
                _vkToHotkeyIndex[(int)e.KeyCode] = index;
                button.Text = e.KeyCode.ToString();
                _isBindingKey = false;
                KeyDown -= handler;
            };

            KeyDown += handler;
        }

        private void ClearHotkeyBinding(int index)
        {
            if (boundKeys[index] != Keys.None)
                _vkToHotkeyIndex.Remove((int)boundKeys[index]);

            boundKeys[index] = Keys.None;
        }

        private Guna.UI2.WinForms.Guna2Button GetHotkeyButton(int index)
        {
            switch (index)
            {
                case 1: return guna2Button35;
                case 2: return guna2Button34;
                case 3: return guna2Button33;
                case 4: return guna2Button31;
                case 5: return guna2Button32;
                case 6: return guna2Button30;
                case 7: return guna2Button4;
                case 8: return guna2Button3;
                case 9: return guna2Button2;
                default: return null;
            }
        }

        private void ResetKeyBinding(int index, Guna.UI2.WinForms.Guna2Button button)
        {
            ClearHotkeyBinding(index);
            button.Text = "-";
        }

        private void ExecuteHotkey(int index)
        {
            if (!EnsureInjected())
                return;

            if (index < 1 || index > 9)
                return;

            var control = GetControlByIndex(index);
            if (control == null)
                return;

            if (control is Guna.UI2.WinForms.Guna2ToggleSwitch toggle)
            {
                toggle.Checked = !toggle.Checked;
            }
            else if (control is Guna.UI2.WinForms.Guna2CustomCheckBox checkBox)
            {
                checkBox.Checked = !checkBox.Checked;
                switch (index)
                {
                    case 7: guna2CustomCheckBox12_Click_1(checkBox, EventArgs.Empty); break;
                    case 8: guna2CustomCheckBox19_Click(checkBox, EventArgs.Empty); break;
                    case 9: guna2CustomCheckBox11_Click(checkBox, EventArgs.Empty); break;
                }
            }
        }

        // ================= BIND HOTKEY BUTTONS =====================

        private void guna2Button35_Click(object sender, EventArgs e) => BindKey(1, guna2Button35);
        private void guna2Button34_Click(object sender, EventArgs e) => BindKey(2, guna2Button34);
        private void guna2Button33_Click(object sender, EventArgs e) => BindKey(3, guna2Button33);
        private void guna2Button31_Click(object sender, EventArgs e) => BindKey(4, guna2Button31);
        private void guna2Button32_Click(object sender, EventArgs e) => BindKey(5, guna2Button32);
        private void guna2Button30_Click(object sender, EventArgs e) => BindKey(6, guna2Button30);
        private void guna2Button4_Click(object sender, EventArgs e) => BindKey(7, guna2Button4);
        private void guna2Button3_Click(object sender, EventArgs e) => BindKey(8, guna2Button3);
        private void guna2Button2_Click(object sender, EventArgs e) => BindKey(9, guna2Button2);


        // ================= RESET HOTKEY BUTTONS =====================

        private void guna2Button35_Clickr(object sender, EventArgs e) => ResetKeyBinding(1, guna2Button35);//
        private void guna2Button34_Clickr(object sender, EventArgs e) => ResetKeyBinding(2, guna2Button34);//
        private void guna2Button33_Clickr(object sender, EventArgs e) => ResetKeyBinding(3, guna2Button33);//

        private void guna2Button31_Clickr(object sender, EventArgs e) => ResetKeyBinding(4, guna2Button31);//
        private void guna2Button32_Clickr(object sender, EventArgs e) => ResetKeyBinding(5, guna2Button32);//
        private void guna2Button30_Clickr(object sender, EventArgs e) => ResetKeyBinding(6, guna2Button30);//

        private void guna2Button4_Clickr(object sender, EventArgs e) => ResetKeyBinding(7, guna2Button4);//
        private void guna2Button3_Clickr(object sender, EventArgs e) => ResetKeyBinding(8, guna2Button3);//
        private void guna2Button2_Clickr(object sender, EventArgs e) => ResetKeyBinding(9, guna2Button2);//





        private void guna2CustomCheckBox37_Click(object sender, EventArgs e)
        {
            bool streamerMode = guna2CustomCheckBox37.Checked;

            // Hide from taskbar if enabled
            base.ShowInTaskbar = !streamerMode;
            MainMenu.SetWindowDisplayAffinity(base.Handle, streamerMode ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);

            try
            {
                // Update configuration
                Config.StreamMode = streamerMode;
                Config.sound = streamerMode;
                Config.Notif();

                // Update all open forms for capture exclusion
                foreach (Form form in Application.OpenForms.Cast<Form>().ToList())
                {
                    if (form.Handle == IntPtr.Zero)
                        continue;
                    SetWindowDisplayAffinity(form.Handle, streamerMode ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
                }

                // Ensure guna2Panel16 is initialized and toggle visibility
                if (guna2Panel1 == null)
                {
                    UpdateStatus("Error: guna2Panel1 is not initialized", Color.Red);
                    return;
                }

                // Store the original visibility state if not already stored
                if (!streamerMode && guna2Panel1.Tag == null)
                    guna2Panel1.Tag = guna2Panel1.Visible;

                // Toggle visibility based on streamerMode
                guna2Panel1.Visible = streamerMode || (guna2Panel1.Tag is bool tagValue && tagValue);

                // Update status text
                UpdateStatus(
                    streamerMode ? "Streamer Mode Enabled (ESP visible to you only)" : "Streamer Mode Disabled (Normal Mode)",
                    streamerMode ? Color.Orange : Color.Green
                );
            }
            catch (Exception ex)
            {
                UpdateStatus($"Streamer Mode Error: {ex.Message}", Color.Red);
            }
        }

        // ================= HOTKEY LISTENER =================
        protected override void WndProc(ref Message m)
        {
            if (m.Msg == WM_HOTKEY)
            {
                int id = m.WParam.ToInt32();

                // INSERT KEY → Toggle guna2Panel1 + Streamer mode
                if (id == INSERT_HOTKEY_ID)
                {
                    bool hide = guna2Panel1.Visible;

                    // Toggle panel visibility
                    guna2Panel1.Visible = !hide;

                    // Streamer mode (hide from capture + taskbar)
                    foreach (Form form in Application.OpenForms)
                    {
                        if (form.Handle == IntPtr.Zero)
                            continue;

                        SetWindowDisplayAffinity(
                            form.Handle,
                            hide ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE
                        );

                        form.ShowInTaskbar = !hide;
                    }

                    // Optional: make form transparent when hidden
                    this.Opacity = hide ? 0.01 : 1.0;
                }
            }

            base.WndProc(ref m);
        }

        // ================= FORM CLOSING CLEANUP =================
        private void MainMenu_FormClosing(object sender, FormClosingEventArgs e)
        {
            UnregisterHotKey(this.Handle, INSERT_HOTKEY_ID);
            _vkToHotkeyIndex.Clear();

            base.OnFormClosing(e);

            if (_hookID != IntPtr.Zero)
                UnhookWindowsHookEx(_hookID);
        }



        // ================= CHECKBOX MAP =====================
        private Control GetControlByIndex(int index)
        {
            switch (index)
            {
                case 1: return guna2ToggleSwitch17;//
                case 2: return guna2ToggleSwitch1;//
                case 3: return guna2ToggleSwitch7;//
                case 4: return guna2ToggleSwitch10;//
                case 5: return guna2ToggleSwitch9;//
                case 6: return guna2ToggleSwitch11;//
                case 7: return guna2CustomCheckBox12;//
                case 8: return guna2CustomCheckBox19;//
                case 9: return guna2CustomCheckBox11;//
                default: return null;
            }
        }


        private void SaveSettings()
        {
            try
            {
                // Dono textboxes ka data array mein daalo
                string[] dataToSave = { Username.Text, password.Text };

                // File.WriteAllLines use karo taaki har value nayi line par jaye
                File.WriteAllLines(configFilePath, dataToSave);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error saving settings: " + ex.Message);
            }
        }
        private void LoadSettings()
        {
            try
            {
                if (File.Exists(configFilePath))
                {
                    string[] savedData = File.ReadAllLines(configFilePath);

                    // Check karo ki file mein kam se kam 2 lines hain ya nahi
                    if (savedData.Length >= 2)
                    {
                        Username.Text = savedData[0].Trim(); // Pehli line Username
                        password.Text = savedData[1].Trim(); // Doosri line Password

                        // Agar data mil gaya toh checkbox ko tick kar do
                        chkRememberMe.Checked = true;
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error loading settings: " + ex.Message);
            }
        }

        private void ToggleNetworkFor4Sec()
        {
            string ruleName = "TemporaryBlock2";

            // Block Network
            BlockNetwork(ruleName);
            SystemSounds.Beep.Play(); // Beep to confirm block

            Thread.Sleep(4000); // Wait for 4 seconds

            // Unblock Network
            UnblockNetwork(ruleName);
            SystemSounds.Beep.Play(); // Beep to confirm unblock
        }

        private void BlockNetwork(string ruleName)
        {
            string[] programs =
            {
                @"C:\Program Files\BlueStacks\HD-Player.exe",
                @"C:\Program Files\BlueStacks_nxt\HD-Player.exe",
                @"C:\Program Files\BlueStacks_msi2\HD-Player.exe",
                @"C:\Program Files\BlueStacks_msi5\HD-Player.exe"
            };

            foreach (var program in programs)
            {
                RunCommand($"netsh advfirewall firewall add rule name=\"{ruleName}\" dir=in action=block program=\"{program}\"");
                RunCommand($"netsh advfirewall firewall add rule name=\"{ruleName}\" dir=out action=block program=\"{program}\"");
            }
        }

        private void UnblockNetwork(string ruleName)
        {
            RunCommand($"netsh advfirewall firewall delete rule name=\"{ruleName}\"");
        }

        private void RunCommand(string command)
        {
            Process.Start(new ProcessStartInfo
            {
                FileName = "cmd.exe",
                Arguments = $"/c {command}",
                WindowStyle = ProcessWindowStyle.Hidden,
                CreateNoWindow = true
            });
        }

        private IntPtr SetHook(LowLevelKeyboardProc proc)
        {
            using (Process curProcess = Process.GetCurrentProcess())
            using (ProcessModule curModule = curProcess.MainModule)
            {
                return SetWindowsHookEx(WH_KEYBOARD_LL, proc, GetModuleHandle(curModule.ModuleName), 0);
            }
        }

        private delegate IntPtr LowLevelKeyboardProc(int nCode, IntPtr wParam, IntPtr lParam);

        private IntPtr HookCallback(int nCode, IntPtr wParam, IntPtr lParam)
        {
            if (nCode >= 0 && !_isBindingKey)
            {
                int message = wParam.ToInt32();
                if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
                {
                    int vkCode = Marshal.ReadInt32(lParam);
                    long tick = Environment.TickCount64;

                    if (vkCode == _lastHotkeyVk && tick - _lastHotkeyTick < 300)
                        return CallNextHookEx(_hookID, nCode, wParam, lParam);

                    if (_vkToHotkeyIndex.TryGetValue(vkCode, out int index))
                    {
                        _lastHotkeyVk = vkCode;
                        _lastHotkeyTick = tick;

                        if (InvokeRequired)
                            BeginInvoke(new Action(() => ExecuteHotkey(index)));
                        else
                            ExecuteHotkey(index);
                    }
                }
            }

            return CallNextHookEx(_hookID, nCode, wParam, lParam);
        }



        private const int WH_KEYBOARD_LL = 13;

        [DllImport("user32.dll")]
        private static extern IntPtr SetWindowsHookEx(int idHook, LowLevelKeyboardProc lpfn, IntPtr hMod, uint dwThreadId);

        [DllImport("user32.dll")]
        private static extern bool UnhookWindowsHookEx(IntPtr hhk);

        [DllImport("user32.dll")]
        private static extern IntPtr CallNextHookEx(IntPtr hhk, int nCode, IntPtr wParam, IntPtr lParam);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        private static extern IntPtr GetModuleHandle(string lpModuleName);

        //}


        private void Form2_Load(object sender, EventArgs e)
        {
            label6.Visible = false;
            label6.Visible = false;
            guna2vSeparator1.Visible = false;
            guna2Separator4.Visible = false;
            guna2vSeparator2.Visible = false;
            guna2Separator5.Visible = false;
            guna2CustomGradientPanel2.Visible = false;
            this.KeyPreview = true;

        }

        // ===== ADDRESSES =====
        private long fireAddress = 0;
        private long speedAddress = 0;
        private bool _mutuallyExclusiveToggleActive = false;

        // ===== SAFE WRITE =====
        private void SafeWrite(long address, string pattern)
        {
            try
            {
                if (address > 0)
                    memoryfast.AobReplace(address, pattern);
            }
            catch { }
        }

        // ===== FIND ADDRESS =====
        private async Task<long> FindAddress(string pattern)
        {
            try
            {
                var result = await memoryfast.AoBScan(pattern);
                if (result != null)
                {
                    foreach (var item in result)
                    {
                        return Convert.ToInt64(item);
                    }
                }
            }
            catch { }
            return 0;
        }

        // ===== LOAD PATCHES =====
        private async Task LoadFunctions()
        {
            if (!memoryfast.SetProcess(new[] { "HD-Player", "HD-Player64", "HD-PlayerMultiInstance", "BlueStacks", "BlueStacks_nxt", "Bluestacks", "BlueStacks X", "BlueStacksX", "MSIAppPlayer", "AppPlayer" }))
                return;

            status.Text = "Loading Functions...";
            fireAddress = await FindAddress("02 2B 07 3D 02 2B 07 3D 02 2B 07 3D");
            speedAddress = await FindAddress("00 00 80 40 33 33 93 40 3D 0A F7 3F");

            this.Invoke(new Action(() =>
            {
                status.Text = fireAddress > 0 && speedAddress > 0
                    ? "Ready"
                    : "Patch Failed";
            }));
        }


        private CancellationTokenSource _cts;

        private void UpdateStatuss(string statusText, Color Color)
        {
            status.Text = statusText;
            //  status.FillColor = Color;
        }

        static IntPtr FindRenderWindow(IntPtr parent)
        {
            IntPtr renderWindow = IntPtr.Zero;
            WinAPI.EnumChildWindows(parent, (hWnd, lParam) =>
            {
                StringBuilder sb = new StringBuilder(256);
                WinAPI.GetWindowText(hWnd, sb, sb.Capacity);
                string windowName = sb.ToString();
                if (!string.IsNullOrEmpty(windowName))
                {
                    if (windowName != "HD-Player")
                    {
                        renderWindow = hWnd;
                    }
                }
                return true;
            }, IntPtr.Zero);

            return renderWindow;
        }



        private void statuslbl_Click(object sender, EventArgs e)
        {

        }

        private async Task<bool> ApplyMemoryPatch(CX memory, string[] searchPatterns, string[] replacePatterns)
        {
            bool success = false;

            for (int i = 0; i < searchPatterns.Length; i++)
            {
                if (string.IsNullOrEmpty(searchPatterns[i]) || string.IsNullOrEmpty(replacePatterns[i])) continue;

                var matches = await memory.AoBScan(searchPatterns[i]);
                if (matches.Any())
                {
                    foreach (long id in matches)
                    {
                        memory.AobReplace(id, replacePatterns[i]);
                    }
                    UpdateStatus($"Memory Patch Applied", Color.Green);
                    success = true;
                }
                else
                {
                    UpdateStatus($"Memory Patch Failed", Color.Red);
                }
            }

            return success;
        }

        private void UpdateStatus(string message, Color color)
        {
            if (status.InvokeRequired)
            {
                status.Invoke(new Action(() =>
                {
                    status.Text = message;
                    status.ForeColor = color;
                }));
            }
            else
            {
                status.Text = message;
                status.ForeColor = color;
            }
        }


        private void UpdateStatus(bool isEnabled, string feature)
        {
            UpdateStatus($"Status : {feature} {(isEnabled ? "Activated" : "Disabled")}", isEnabled ? Color.Green : Color.Red);
        }



        private void chkRememberMe_Click(object sender, EventArgs e)
        {
            if (chkRememberMe.Checked)
            {
                File.WriteAllText(configFilePath, guna2TextBox1.Text); // Save only the entered text
                SaveSettings();
            }
            else
            {
                guna2TextBox1.Clear(); // Clear the text
                if (File.Exists(configFilePath))
                {
                    File.Delete(configFilePath); // Delete the file
                }
            }

        }




        private void guna2CustomCheckBox13_Click(object sender, EventArgs e)
        {

        }

        private async void guna2CustomCheckBox21_Click(object sender, EventArgs e)
        {
            status.Text = "Applying...";
            string[] processName = { "HD-Player", "HD-Player64", "HD-PlayerMultiInstance", "BlueStacks", "BlueStacks_nxt", "Bluestacks", "BlueStacks X", "BlueStacksX", "MSIAppPlayer", "AppPlayer" };
            bool success = memoryfast.SetProcess(processName);
            if (!success)
            {
                return;
            }

            DialogResult dialogWallhack = MessageBox.Show("Do you want to turn on Wallhack ?", "Wallhack Injection", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            bool injectWallhack = (dialogWallhack == DialogResult.Yes);

            DialogResult dialogMacroV9 = MessageBox.Show("Do you want to turn on SNIPER SWITCH , GLITCH FIRE ?", "SNIPER SWITCH , GLITCH FIRE Activation", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            bool injectMacroV9 = (dialogMacroV9 == DialogResult.Yes);

            // List to store selected modifications
            List<(string scan, string replace)> modificationsToApply = new List<(string scan, string replace)>();

            // If Macro V9 is selected, inject Macro V9
            if (injectMacroV9)
            {
                // If Macro V9 is not selected, add other normal modifications
                modificationsToApply.AddRange(new (string scan, string replace)[]
                {
                    ("B4 C8 D6 3F 00 00 80 3F 00 00 80 3F 0A D7 A3 3D 00 00 00 00 00 00 5C 43 00 00 90 42 00 00 B4 42 96 00 00 00 00 00 00 00 00 00 00 3F 00 00 80 3E 00 00 00 00 04 00 00 00 00 00 80 3F", "B4 C8 D6 3F 00 00 80 3F 00 00 80 3F 0A D7 A3 3D 00 00 00 00 00 00 5C 43 00 00 90 42 00 00 B4 42 96 00 00 00 00 00 00 00 00 00 00 3c 00 00 80 3c 00 00 00 00 04 00 00 00 00 00 80 3F"),// SNIPER SWITCH
                    ("C0 3F 00 00 00 3F 00 00 80 3F 00 00 00 40 CD CC CC 3D 01 00 00 00 CD CC CC 3D 01", "00 00"), // GLITCH FIRE
                });
            }

            // If Wallhack is selected
            if (injectWallhack)
            {
                modificationsToApply.AddRange(new (string scan, string replace)[]
                {
        ("53 FE FF CA 0A 00 00 EA 00 00 94 E5 7F 2F 8D E2 01 10 A0 E3 00 30 90 E5 04 00 A0 E1 33 FF 2F E1 00 00 50 E3",
         "00 C0 79 44 0A 00 00 EA 00 00 94 E5"), // WALLHACK1

        ("3F AE 47 81 3F 00 1A B7 EE DC 3A 9F ED 30 00 4F E2 43 2A B0 EE EF 0A 60 F4 43 6A F0 EE 1C 00 8A E2 43 5A F0 EE 8F 0A 48 F4 43 2A F0 EE 43 7A B0 EE 8F 0A 40 F4 41 AA B0",
         "BF AE 47 81 3F 00 1A B7 EE DC 3A 9F ED 30 00 4F E2 43 2A B0 EE EF 0A 60 F4 43 6A F0 EE 1C 00 8A E2 43 5A F0 EE 8F 0A 48 F4 43 2A F0 EE 43 7A B0 EE 8F 0A 40 F4 41 AA B0"), // WALLHACK2

        ("0D FA 9D ED 04 DA 68 EE A8 CA 66 EE 8F EA 65 EE A1 FA 6A EE AB BA 7C EE 8C CA 69 EE",
         "0D FA 9D 6D 04 DA 68 EE A8 CA 66 EE 8F EA 65 EE A1 FA 6A EE AB BA 7C EE 8C CA 69 EE") // WALLHACK3
                });
            }

            // Apply the selected modifications
            foreach (var pair in modificationsToApply)
            {
                if (!string.IsNullOrEmpty(pair.scan) && !string.IsNullOrEmpty(pair.replace))
                {
                    IEnumerable<long> result = await memoryfast.AoBScan(pair.scan);
                    foreach (long id in result)
                    {
                        memoryfast.AobReplace(id, pair.replace);
                    }
                }
            }

            // Update status label
            if (injectMacroV9)
            {
                status.Text = "SNIPER SWITCH , GLITCH FIRE Activated!";
            }
            else if (injectWallhack)
            {
                status.Text = "Wallhack Activated!";
            }
            else
            {
                status.Text = "Changes Applied!";
            }

        }



        //Mem Fahim = new Mem();


        static void DeleteFirewallRule(string programPath)
        {
            ExecuteCommand("netsh advfirewall firewall delete rule name=all program=\"" + programPath + "\"");
        }
        static void ExecuteCommand(string command)
        {
            ProcessStartInfo startInfo = new ProcessStartInfo
            {
                FileName = "cmd.exe",
                Arguments = "/c " + command,
                RedirectStandardOutput = true,
                UseShellExecute = false,
                CreateNoWindow = true
            };

            Process process = new Process { StartInfo = startInfo };
            process.Start();
            process.WaitForExit();
        }



        private void guna2CustomCheckBox10_Click(object sender, EventArgs e)
        {

        }








        private void guna2CustomCheckBox32_Click(object sender, EventArgs e)
        {



        }



        private void guna2TrackBar3_Scroll(object sender, ScrollEventArgs e)
        {

        }


        private void guna2CustomCheckBox12_Click(object sender, EventArgs e)
        {


        }



        private async void guna2CustomCheckBox22_Click(object sender, EventArgs e)
        {


        }

        private async void guna2CustomCheckBox21_Click_3(object sender, EventArgs e)
        {


            //string processName = "HD-Player";
            //UpdateStatus("Applying NO RECOIL, Please Wait...", Color.Green);

            //CX memoryfast = new CX();

            //if (!memoryfast.SetProcess(new[] { processName }))
            //{
            //    UpdateStatus("Status : Process Not Found", Color.Red);
            //    MessageBox.Show("Error: Process Not Found", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            //    guna2CustomCheckBox6.Checked = false;
            //    return;
            //}

            //// ORIGINAL (disabled) bytes
            //string originalPattern = "10 0A 18 EE 02 8B BD EC 30 88 BD E8 30 48 2D E9 08 B0 8D E2 00 40 A0 E1 DC";
            //// PATCHED (enabled) bytes
            //string patchedPattern = "10 0A 18 EE 02 8B BD EC 30 88 BD E8 FF 00 45 E3 1E FF 2F E1";

            //try
            //{
            //    await Task.Run(async () =>
            //    {
            //        try
            //        {
            //            bool success;

            //            if (guna2CustomCheckBox6.Checked)
            //            {
            //                // Enable: replace original -> patched
            //                success = await ApplyMemoryPatch(memoryfast, new[] { originalPattern }, new[] { patchedPattern });
            //            }
            //            else
            //            {
            //                // Disable: replace patched -> original (restore)
            //                success = await ApplyMemoryPatch(memoryfast, new[] { patchedPattern }, new[] { originalPattern });
            //            }

            //            Invoke(new Action(() =>
            //            {
            //                if (success)
            //                {
            //                    UpdateStatus(guna2CustomCheckBox6.Checked ? "NO RECOIL Enabled" : "NO RECOIL Disabled", Color.Green);
            //                }
            //                else
            //                {
            //                    UpdateStatus("Failed to Apply Patch", Color.Red);
            //                    // rollback checkbox state so UI matches actual memory
            //                    guna2CustomCheckBox6.Checked = !guna2CustomCheckBox31.Checked;
            //                }
            //            }));
            //        }
            //        catch (Exception ex)
            //        {
            //            Invoke(new Action(() =>
            //            {
            //                UpdateStatus($"Error - {ex.Message}", Color.Red);
            //                // safe rollback
            //                guna2CustomCheckBox6.Checked = false;
            //            }));
            //        }
            //    });
            //}
            //catch (Exception ex)
            //{
            //    UpdateStatus($"Error - {ex.Message}", Color.Red);
            //    guna2CustomCheckBox6.Checked = false;
            //}


        }








        //private void guna2CustomCheckBox43_Click(object sender, EventArgs e)
        //{
        //    if (guna2CustomCheckBox43.Checked)
        //    {
        //        status.Text = "Net Block Done";

        //        ExecuteCommand("netsh advfirewall firewall add rule name=\"TemporaryBlock0\" dir=in action=block profile=any program=\"C:\\Program Files\\BlueStacks_nxt\\HD-Player.exe\"");
        //        ExecuteCommand("netsh advfirewall firewall add rule name=\"TemporaryBlock0\" dir=out action=block profile=any program=\"C:\\Program Files\\BlueStacks_nxt\\HD-Player.exe\"");
        //        ExecuteCommand("netsh advfirewall firewall add rule name=\"TemporaryBlock1\" dir=in action=block profile=any program=\"C:\\Program Files\\BlueStacks_msi5\\HD-Player.exe\"");
        //        ExecuteCommand("netsh advfirewall firewall add rule name=\"TemporaryBlock1\" dir=out action=block profile=any program=\"C:\\Program Files\\BlueStacks_msi5\\HD-Player.exe\"");
        //        ExecuteCommand("netsh advfirewall firewall add rule name=\"TemporaryBlock2\" dir=in action=block profile=any program=\"C:\\Program Files\\BlueStacks\\HD-Player.exe\"");
        //        ExecuteCommand("netsh advfirewall firewall add rule name=\"TemporaryBlock2\" dir=out action=block profile=any program=\"C:\\Program Files\\BlueStacks\\HD-Player.exe\"");
        //        ExecuteCommand("netsh advfirewall firewall add rule name=\"TemporaryBlock3\" dir=in action=block profile=any program=\"C:\\Program Files\\BlueStacks_msi2\\Bluestacks.exe\"");
        //        ExecuteCommand("netsh advfirewall firewall add rule name=\"TemporaryBlock3\" dir=in action=block profile=any program=\"C:\\Program Files\\BlueStacks_msi2\\HD-Player.exe\"");
        //        ExecuteCommand("netsh advfirewall firewall add rule name=\"TemporaryBlock3\" dir=out action=block profile=any program=\"C:\\Program Files\\BlueStacks_msi2\\HD-Player.exe\"");

        //    }
        //    else
        //    {
        //        DeleteFirewallRule("C:\\Program Files\\BlueStacks_nxt\\HD-Player.exe");
        //        DeleteFirewallRule("C:\\Program Files\\BlueStacks_msi5\\HD-Player.exe");
        //        DeleteFirewallRule("C:\\Program Files\\BlueStacks\\HD-Player.exe");
        //        DeleteFirewallRule("C:\\Program Files\\BlueStacks_msi2\\HD-Player.exe");
        //        status.Text = "Net Unblock Done";

        //    }
        //}


        //// Purane saare variables aur functions hata do: _patchLock, _patchedAddresses, _isPatchingInProgress, NARZO, _searchPatterns, _replacePatterns, ApplyPatchesAsync, RestorePatchesAsync

        //// Ab tera button event bahut saaf aur simple hoga
        //private async void guna2ToggleSwitch12_CheckedChanged(object sender, EventArgs e)
        //{
        //    Config.Notif(); // Teri notification

        //    if (guna2ToggleSwitch12.Checked)
        //    {
        //        // Fast Fire enable karo
        //        bool success = await FastFire.ApplyAsync();
        //        if (success)
        //        {
        //            status.Text = "Fast Fire Enabled!";
        //            status.ForeColor = Color.Lime;
        //        }
        //        else
        //        {
        //            // Agar fail ho jaye toh switch wapas off kar do
        //            guna2ToggleSwitch12.Checked = false;
        //            status.Text = "Fast Fire Failed!";
        //            status.ForeColor = Color.Red;
        //        }
        //    }
        //    else
        //    {
        //        // Fast Fire disable karo
        //        bool success = await FastFire.RestoreAsync();
        //        status.Text = "Fast Fire Disabled!";
        //        status.ForeColor = Color.Red;
        //    }
        //}

        private bool _fastFireToggleInternal;
        private bool _speedToggleInternal;

        private void SetSpeedToggleUi(bool value)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(() => SetSpeedToggleUi(value)));
                return;
            }

            _speedToggleInternal = true;
            guna2ToggleSwitch17.Checked = value;
            _speedToggleInternal = false;
        }

        private void SetFastFireToggleUi(bool value)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(() => SetFastFireToggleUi(value)));
                return;
            }

            _fastFireToggleInternal = true;
            guna2ToggleSwitch12.Checked = value;
            _fastFireToggleInternal = false;
        }

        private void UpdatePatchStatusSafe(string message)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(() => UpdatePatchStatusSafe(message)));
                return;
            }

            status.Text = message;
        }

        //private void SyncSpeedFastFireUi()
        //{
        //    if (InvokeRequired)
        //    {
        //        BeginInvoke(SyncSpeedFastFireUi);
        //        return;
        //    }

        //    SetSpeedToggleUi(LobbyPatchController.SpeedEnabled);
        //    SetFastFireToggleUi(LobbyPatchController.FastFireEnabled);

        //    if (!LobbyPatchController.SpeedEnabled && !LobbyPatchController.FastFireEnabled)
        //        status.Text = "Speed/Fast Fire reset (Lobby)";
        //}

        // private async void guna2ToggleSwitch12_CheckedChanged(object sender, EventArgs e)
        //  {
        //if (_fastFireToggleInternal)
        //    return;

        //Config.Notif();

        //bool wantOn = guna2ToggleSwitch12.Checked;

        //if (wantOn && !LobbyPatchController.IsPreloaded)
        //{
        //    SetFastFireToggleUi(false);
        //    status.Text = "Fast Fire not loaded yet!";
        //    return;
        //}

        //bool ok = await LobbyPatchController.SetFastFireAsync(wantOn, () => SetSpeedToggleUi(false));

        //if (wantOn && !ok)
        //{
        //    SetFastFireToggleUi(false);
        //    status.Text = "Fast Fire Failed!";
        //    return;
        //}

        //status.Text = wantOn ? "Fast Fire ON!" : "Fast Fire OFF!";
        //   }

        //private async void guna2ToggleSwitch17_CheckedChanged(object sender, EventArgs e)
        //{
        //    if (_speedToggleInternal)
        //        return;

        //    Config.Notif();

        //    bool wantOn = guna2ToggleSwitch17.Checked;

        //    if (wantOn && !LobbyPatchController.IsPreloaded)
        //    {
        //        SetSpeedToggleUi(false);
        //        status.Text = "Speed not loaded yet!";
        //        return;
        //    }

        //    bool ok = await LobbyPatchController.SetSpeedAsync(wantOn, () => SetFastFireToggleUi(false));

        //    if (wantOn && !ok)
        //    {
        //        SetSpeedToggleUi(false);
        //        status.Text = "Speed Hack Failed!";
        //        return;
        //    }

        //    status.Text = wantOn ? "Speed ON!" : "Speed OFF!";


        //}
        private void guna2ToggleSwitch12_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void guna2ToggleSwitch17_CheckedChanged(object sender, EventArgs e)
        {
            if (_mutuallyExclusiveToggleActive) return;

            if (!memoryfast.SetProcess(new[] { "HD-Player", "HD-Player64", "HD-PlayerMultiInstance", "BlueStacks", "BlueStacks_nxt", "Bluestacks", "BlueStacks X", "BlueStacksX", "MSIAppPlayer", "AppPlayer" }))
            {
                guna2ToggleSwitch17.Checked = false;
                return;
            }

            // ===== FAST FIRE =====
            if (guna2ToggleSwitch17.Checked)
            {
                _mutuallyExclusiveToggleActive = true;
                guna2ToggleSwitch10.Checked = false;
                _mutuallyExclusiveToggleActive = false;

                // speed patch
                if (speedAddress > 0)
                {
                    SafeWrite(
                        speedAddress,
                        "00 00 80 40 00 00 80 40 CB D2 4D 3E"
                    );
                }

                // fire patch
                if (fireAddress > 0)
                {
                    SafeWrite(
                        fireAddress,
                        "08 39 60 3B 08 39 60 3B 08 39 60 3B"
                    );
                }

                status.Text = "Fast Fire ON";
            }
            else
            {
                // restore speed
                if (speedAddress > 0)
                {
                    SafeWrite(
                        speedAddress,
                        "00 00 80 40 33 33 93 40 66 66 06 40"
                    );
                }

                // if global speed still enabled
                if (guna2ToggleSwitch10.Checked)
                {
                    if (fireAddress > 0)
                    {
                        SafeWrite(
                            fireAddress,
                            "02 2B AA 3C 02 2B AA 3C 02 2B 07 3D"
                        );
                    }
                }
                else
                {
                    // fully restore
                    if (fireAddress > 0)
                    {
                        SafeWrite(
                            fireAddress,
                            "02 2B 07 3D 02 2B 07 3D 02 2B 07 3D"
                        );
                    }
                }

                status.Text = "Fast Fire OFF";
            }
        }

        private void guna2Button14_Click(object sender, EventArgs e)
        {
            Config.Notif();
            this.WindowState = FormWindowState.Minimized;
        }



        private void guna2Panel25_Paint(object sender, PaintEventArgs e)
        {

        }

        private void guna2Panel24_Paint(object sender, PaintEventArgs e)
        {

        }




        bool autoReset = false;

        private async void guna2ToggleSwitch1_CheckedChanged(object sender, EventArgs e)
        {
            if (autoReset) return;

            if (guna2ToggleSwitch1.Checked)
            {
                EnsureFeatureThread("Map_Teleport", Map_Teleport.Work);
                Config.teleportmap = true;

                status.Text = "Status : TP MARK ENABLED";
                status.ForeColor = Color.LimeGreen;

                // 🔥 yaha apna TP MARK ka logic call kar
                // DoTeleportMark();

                await Task.Delay(50); // thoda smooth delay (optional)

                // 🔁 auto OFF
                autoReset = true;
                guna2ToggleSwitch1.Checked = false;
                autoReset = false;

                Config.teleportmap = false;

                status.Text = "Status : TP MARK DISABLED";
                status.ForeColor = Color.Red;
            }
        }




        private void guna2ToggleSwitch14_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void guna2ToggleSwitch13_CheckedChanged(object sender, EventArgs e)
        {

        }


        private async void guna2ToggleSwitch11_CheckedChanged(object sender, EventArgs e)
        {
            //Config.Notif();
            //string processName = "HD-Player";
            //UpdateStatus("Applying Fly Hack, Please Wait...", Color.Green);

            //CX memoryfast = new CX();

            //if (!memoryfast.SetProcess(new[] { processName }))
            //{
            //    UpdateStatus("Status : Process Not Found", Color.Red);
            //    MessageBox.Show("Error: Process Not Found", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            //    return;
            //}

            //string originalPattern = "AC C5 27 37 00 10 A0 E1";
            //string modifiedPattern = "AC C5 A9 3F 00 10 A0 E1";

            //string[] searchPatterns;
            //string[] replacePatterns;

            //if (guna2ToggleSwitch11.Checked)
            //{
            //    searchPatterns = new[] { originalPattern };
            //    replacePatterns = new[] { modifiedPattern };
            //}
            //else
            //{
            //    searchPatterns = new[] { modifiedPattern };
            //    replacePatterns = new[] { originalPattern };
            //}

            //try
            //{
            //    bool success = await ApplyMemoryPatch(memoryfast, searchPatterns, replacePatterns);

            //    if (success)
            //    {
            //        if (guna2ToggleSwitch11.Checked)
            //            UpdateStatus("Fly Hack Enabled", Color.Green);
            //        else
            //            UpdateStatus("Fly Hack Disabled", Color.Yellow);
            //    }
            //    else
            //    {
            //        UpdateStatus("Failed to Apply Patch", Color.Red);
            //    }
            //}
            //catch (Exception ex)
            //{
            //    UpdateStatus($"Error - {ex.Message}", Color.Red);
            //}
            Config.Notif();
            Config.FlyHackint = guna2ToggleSwitch11.Checked;
            if (guna2ToggleSwitch11.Checked)
                EnsureFeatureThread("FlyHack", FlyHack_LocalPlayer2.Start);
            status.Text = guna2ToggleSwitch11.Checked
                ? "Fly Hack 80x Enabled!"
                : "Fly Hack 80x Disabled!";
        }

        private async void guna2ToggleSwitch10_CheckedChanged(object sender, EventArgs e)
        {
            if (_mutuallyExclusiveToggleActive) return;

            if (!memoryfast.SetProcess(new[] { "HD-Player", "HD-Player64", "HD-PlayerMultiInstance", "BlueStacks", "BlueStacks_nxt", "Bluestacks", "BlueStacks X", "BlueStacksX", "MSIAppPlayer", "AppPlayer" }))
            {
                guna2ToggleSwitch10.Checked = false;
                return;
            }

            // ===== GLOBAL SPEED =====
            if (guna2ToggleSwitch10.Checked)
            {
                _mutuallyExclusiveToggleActive = true;
                guna2ToggleSwitch17.Checked = false;
                _mutuallyExclusiveToggleActive = false;

                if (fireAddress > 0)
                {
                    SafeWrite(
                        fireAddress,
                        "02 2B AA 3C 02 2B AA 3C 02 2B 07 3D"
                    );
                }

                status.Text = "Global Speed ON";
            }
            else
            {
                // restore normal
                if (fireAddress > 0)
                {
                    SafeWrite(
                        fireAddress,
                        "02 2B 07 3D 02 2B 07 3D 02 2B 07 3D"
                    );
                }

                status.Text = "Global Speed OFF";
            }
        }


        private void guna2ToggleSwitch7_CheckedChanged(object sender, EventArgs e)
        {
            Config.Notif();
            Config.ClimbUpEnabled = guna2ToggleSwitch7.Checked;
            if (guna2ToggleSwitch7.Checked)
                EnsureFeatureThread("ClimbUp", ClimbUp.Start);
            status.Text = guna2ToggleSwitch7.Checked
                ? "Climb Up Enabled!"
                : "Climb Up Disabled!";
        }



        private void guna2ToggleSwitch9_CheckedChanged(object sender, EventArgs e)
        {
            if (guna2ToggleSwitch9.Checked)
            {
                Downplayer.Start();
                divekill.Start();

                status.Text = "DiveKill Enabled!";
                status.ForeColor = Color.Lime;
            }
            else
            {
                Downplayer.Stop();
                divekill.Stop();

                status.Text = "DiveKill Disabled!";
                status.ForeColor = Color.Red;
            }
        }



        private void pictureBox4_Click(object sender, EventArgs e)
        {
            Config.Notif();
            this.WindowState = FormWindowState.Minimized;
        }

        private void BRUTALF_Paint(object sender, PaintEventArgs e)
        {

        }

        private void guna2CustomCheckBox12_Click_1(object sender, EventArgs e)
        {
            Config.success();
            Config.AimbotVisible = guna2CustomCheckBox12.Checked;
            if (guna2CustomCheckBox12.Checked)
                EnsureFeatureThread("AimbotAi", AimbotAi.Work);
            status.Text = "Aimbot Enabled !";

        }

        private void Sign_Click(object sender, EventArgs e)
        {

        }
        private void Main_Click(object sender, EventArgs e)
        {
            if (!isLoggedIn)
            {
                info.Show("Please login first");
                return;
            }
            main1.Visible = true;
            brutal.Visible = false;
            esp1.Visible = false;
            about1.Visible = false;
            Main.ForeColor = Color.Red;
            Visuals.ForeColor = Color.Gray;
            Extras.ForeColor = Color.Gray;
            About.ForeColor = Color.Gray;

        }


        private void guna2ControlBox3_Click(object sender, EventArgs e)
        {
            this.WindowState = FormWindowState.Minimized;
        }

        //========================================================================================================//
        //========================================================================================================//
        //============================== INJECT METHOD START =====================================================//
        //========================================================================================================//

        enum InjectResult
        {
            Success,
            GameNotFound,
            Failed
        }

        private async void guna2Button5_Click_1(object sender, EventArgs e)
        {
            if (connected)
                return;
            guna2Button5.Enabled = false;
            try
            {
                InjectResult result = await TryInjectAsync();

                if (result == InjectResult.Success)
                {
                    LoadFunctions();

                    UpdateStatuss("Connected", Color.Lime);
                    guna2Button5.Enabled = false;

                    return;
                }
                else if (result == InjectResult.GameNotFound)
                {
                    UpdateStatuss("Game Not Found", Color.Red);

                    MessageBox.Show(
                        "Free Fire or Free Fire MAX is not running.",
                        "Injection Failed",
                        MessageBoxButtons.OK,
                        MessageBoxIcon.Warning
                    );
                }
                else
                {
                    UpdateStatuss("Injection Failed", Color.Red);
                }
            }
            finally
            {
                if (!connected)
                    guna2Button5.Enabled = true;
            }
        }

        private async Task<InjectResult> TryInjectAsync()
        {
            if (connected)
                return InjectResult.Success;
            try
            {
                if (Offsets.Il2Cpp == 0)
                    return InjectResult.GameNotFound;
                SoundManager.PlayInject();
                Core.Handle = FindRenderWindow(mainHandle);
                var esp = new ESP();
                await esp.Start();
                StartCheatThreads();
                connected = true;
                startk = true;
                Config.IsInjected = true;
                SoundManager.PlaySuccess();
                return InjectResult.Success;
            }
            catch (Exception ex)
            {
                SoundManager.PlayFail();
                UpdateStatuss("Injection Failed!", Color.Red);
                MessageBox.Show(ex.Message, "Injection Error");
                return InjectResult.Failed;
            }
        }

        private void EnsureFeatureThread(string key, Action start)
        {
            CheatThreadManager.EnsureRunning(key, start);
        }

        private void StartCheatThreads()
        {
            new Thread(Data.Work) { IsBackground = true }.Start();
        }
        //========================================================================================================//
        //========================================================================================================//
        //============================== INJECT METHOD END =====================================================//
        //========================================================================================================//
        //========================================================================================================//




        private void Visuals_Click(object sender, EventArgs e)
        {
            if (!isLoggedIn)
            {
                info.Show("Please login first");
                return;
            }
            main1.Visible = false;
            brutal.Visible = false;
            esp1.Visible = true;
            about1.Visible = false;
            Main.ForeColor = Color.Gray;
            Visuals.ForeColor = Color.Red;
            Extras.ForeColor = Color.Gray;
            About.ForeColor = Color.Gray;

        }

        private void Extras_Click(object sender, EventArgs e)
        {
            if (!isLoggedIn)
            {
                info.Show("Please login first");
                return;
            }
            main1.Visible = false;
            brutal.Visible = true;
            esp1.Visible = false;
            about1.Visible = false;
            Main.ForeColor = Color.Gray;
            Visuals.ForeColor = Color.Gray;
            Extras.ForeColor = Color.Red;
            About.ForeColor = Color.Gray;

        }

        private void About_Click(object sender, EventArgs e)
        {
            if (!isLoggedIn)
            {
                info.Show("Please login first");
                return;
            }
            main1.Visible = false;
            brutal.Visible = false;
            esp1.Visible = false;
            about1.Visible = true;
            Main.ForeColor = Color.Gray;
            Visuals.ForeColor = Color.Gray;
            Extras.ForeColor = Color.Gray;
            About.ForeColor = Color.Red;

        }
        public static api KeyAuthApp = new api(
         name: "CHEATSEXESILENTAIM",
         ownerid: "OrGcs1PvtB",
         secret: "981f6b0f4a4b4e54769efd15810019762d5cb3960bde4e783ea12a6faa90633c",
         version: "1.0"
        );
        private async void LoginSuccessfull()
        {
            if (!initSuccess)
            {
                info.Show("KeyAuth not initialized. Restart app.");
                return;
            }

            try
            {
                KeyAuthApp.login(
                    Username.Text.Trim(),
                    password.Text.Trim()
                );

                if (KeyAuthApp.response.success)
                {
                    isLoggedIn = true;
                    Session.Username = KeyAuthApp.user_data.username;

                    if (KeyAuthApp.user_data.subscriptions != null &&
                        KeyAuthApp.user_data.subscriptions.Count > 0)
                    {
                        string expiryUnix = KeyAuthApp.user_data.subscriptions[0].expiry;
                        Session.Expiry = UnixTimeToDate(expiryUnix);
                        Session.Subscription = KeyAuthApp.user_data.subscriptions[0].subscription;
                    }
                    else
                    {
                        Session.Expiry = "No Subscription";
                        Session.Subscription = "N/A";
                    }

                    UpdateUserUI();
                    info.Show("Login SuccessFull");
                    SoundManager.PlaySuccess();
                    login.Visible = false;
                    Sign.ForeColor = Color.Gray;
                }
                else
                {
                    isLoggedIn = false;
                    string failMsg = string.IsNullOrEmpty(KeyAuthApp.response.message)
                        ? "Login Failed"
                        : KeyAuthApp.response.message;

                    info.Show(failMsg);
                }
            }
            catch (Exception ex)
            {
                isLoggedIn = false;
                info.Show("Login Error: " + ex.Message);
            }
        }

        private async void LicenseLoginSuccessfull()
        {
            if (!initSuccess)
            {
                info.Show("KeyAuth not initialized. Restart app.");
                Debug.WriteLine("[KeyAuth] License login blocked — init was not successful.");
                return;
            }

            try
            {
                KeyAuthApp.license(guna2TextBox3.Text.Trim());
            }
            catch (Exception ex)
            {
                Debug.WriteLine("[KeyAuth] License EXCEPTION: " + ex.Message);
                info.Show("License error: " + ex.Message);
                return;
            }

            if (KeyAuthApp.response.success)
            {
                isLoggedIn = true;

                // ✅ username save
                Session.Username = KeyAuthApp.user_data.username;

                // ✅ expiry save
                if (KeyAuthApp.user_data.subscriptions != null &&
                    KeyAuthApp.user_data.subscriptions.Count > 0)
                {
                    string expiryUnix = KeyAuthApp.user_data.subscriptions[0].expiry;
                    Session.Expiry = UnixTimeToDate(expiryUnix);
                    Session.Subscription = KeyAuthApp.user_data.subscriptions[0].subscription;
                }
                else
                {
                    Session.Expiry = "No Subscription";
                    Session.Subscription = "N/A";
                }

                UpdateUserUI();

                info.Show("License Login SuccessFull");
                SoundManager.PlaySuccess();
                login.Visible = false;
                Sign.ForeColor = Color.Gray;
            }
            else
            {
                isLoggedIn = false;

                string failMsg = string.IsNullOrEmpty(KeyAuthApp.response.message)
                    ? "Invalid License Key"
                    : KeyAuthApp.response.message;

                Debug.WriteLine("[KeyAuth] License failed: " + failMsg);
                info.Show(failMsg);
            }
        }

        private void UpdateUserUI()
        {
            if (isLoggedIn)
            {
                lblUsername.Text = Session.Username;
                label57.Text = Session.Subscription;

                string rawExpiry = Session.Expiry;
                bool isLifetime = rawExpiry.Contains("9999");

                // Set Expiry text
                if (isLifetime)
                {
                    lblExpiry.Text = $"{rawExpiry} (Lifetime subscription type)";
                    lblExpiry.ForeColor = Color.White;
                }
                else
                {
                    lblExpiry.Text = rawExpiry;
                    lblExpiry.ForeColor = Color.White;
                }

                // Calculate Days Left
                DateTime expDate;
                if (DateTime.TryParseExact(rawExpiry, "dd/MM/yyyy", null, System.Globalization.DateTimeStyles.None, out expDate))
                {
                    int daysLeft = (expDate.Date - DateTime.Now.Date).Days;
                    if (daysLeft < 0)
                    {
                        lblDaysLeft.Text = "Expired";
                        lblDaysLeft.ForeColor = Color.Red;
                    }
                    else if (daysLeft == 0)
                    {
                        lblDaysLeft.Text = "Expires Today";
                        lblDaysLeft.ForeColor = Color.OrangeRed;
                    }
                    else
                    {
                        if (isLifetime || daysLeft > 100000)
                        {
                            int years = (int)(daysLeft / 365.25);
                            lblDaysLeft.Text = $"{daysLeft} Days left (approx. {years} years)";
                            lblDaysLeft.ForeColor = Color.Lime;
                        }
                        else
                        {
                            lblDaysLeft.Text = $"{daysLeft} Days left";
                            if (daysLeft <= 3)
                                lblDaysLeft.ForeColor = Color.Orange;
                            else
                                lblDaysLeft.ForeColor = Color.Lime;
                        }
                    }
                }
                else
                {
                    lblDaysLeft.Text = "N/A";
                    lblDaysLeft.ForeColor = Color.Gray;
                }
            }
            else
            {
                lblUsername.Text = ".....................";
                label57.Text = ".....................";
                lblExpiry.Text = ".....................";
                lblDaysLeft.Text = ".....................";
                lblExpiry.ForeColor = Color.White;
                lblDaysLeft.ForeColor = Color.White;
            }
        }

        private void guna2Button1_Click(object sender, EventArgs e)
        {
            LicenseLoginSuccessfull();
        }

        private void guna2Button17_Click(object sender, EventArgs e)
        {
            LoginSuccessfull();
        }
        public string UnixTimeToDate(string unixTime)
        {
            long seconds = long.Parse(unixTime);
            DateTime date = DateTimeOffset.FromUnixTimeSeconds(seconds).DateTime;
            return date.ToString("dd/MM/yyyy");
        }

        public static string GetExpiryWithDays(string expiryDate)
        {
            if (expiryDate == "No Subscription")
                return "No Subscription";

            DateTime exp;
            if (!DateTime.TryParseExact(
                expiryDate,
                "dd/MM/yyyy",
                null,
                System.Globalization.DateTimeStyles.None,
                out exp))
            {
                return "Invalid Expiry";
            }

            int daysLeft = (exp.Date - DateTime.Now.Date).Days;

            if (daysLeft < 0)
                return "Expired";

            if (daysLeft == 0)
                return "Expires Today";

            if (daysLeft > 100000 || expiryDate.Contains("8888"))
                return $"{expiryDate} ({daysLeft} Days left - Lifetime)";

            return $"{expiryDate} ({daysLeft} Days left)";
        }

        private void lblExpiry_Click(object sender, EventArgs e)
        {
            //if (!isLoggedIn)
            //{
            //    lblExpiry.Text = "Not logged in";
            //    lblExpiry.ForeColor = Color.Gray;
            //    return;
            //}

            //string result = GetExpiryWithDays(Session.Expiry);
            //lblExpiry.Text = $"Expiry : {result}";

            //// 🎨 COLOR LOGIC
            //if (result.Contains("Expired"))
            //    lblExpiry.ForeColor = Color.Red;
            //else if (result.Contains("Expires Today"))
            //    lblExpiry.ForeColor = Color.OrangeRed;
            //else
            //{
            //    // extract days number
            //    int days = int.Parse(
            //        System.Text.RegularExpressions.Regex
            //        .Match(result, @"\d+")
            //        .Value
            //    );

            //    if (days <= 3)
            //        lblExpiry.ForeColor = Color.Orange;
            //    else
            //        lblExpiry.ForeColor = Color.Lime;
            //}
        }
        private void lblUsername_Click(object sender, EventArgs e)
        {
            //if (!isLoggedIn)
            //{
            //    lblUsername.Text = "Guest";
            //    return;
            //}

            //lblUsername.Text = $"User : {Session.Username}";
        }

        private void guna2CustomCheckBox8_Click(object sender, EventArgs e)
        {
            if (guna2CustomCheckBox8.Checked)
            {
                // Dono values ko ek array ya list mein rakho
                string[] credentials = {
                     Username.Text,
                     password.Text
                };

                // File mein dono lines save ho jayengi
                File.WriteAllLines(configFilePath, credentials);

                SaveSettings();
                status.Text = "Login details saved!";
            }
            else
            {
                // Uncheck karne par fields clear aur file delete
                Username.Clear();
                password.Clear();

                if (File.Exists(configFilePath))
                {
                    File.Delete(configFilePath);
                }
                status.Text = "Login details removed.";
            }
        }

        private void guna2TrackBar5_Scroll(object sender, ScrollEventArgs e)
        {
            var aimfov1 = guna2TrackBar5.Value;
            aimfovtxt.Text = $"{aimfov1}";
            Config.AimBotFov = aimfov1;
            UpdateStatus(true, $"Aim Fov Range : {Config.AimBotFov}");  // Displaying the FOV value in the status message
        }



        private void guna2TrackBar6_Scroll(object sender, ScrollEventArgs e)
        {
            var DELAY = guna2TrackBar6.Value;
            smooth.Text = $"{DELAY}";
            Config.AimbotSmoothness = DELAY;
            UpdateStatus(true, $"Aim Smooth: {Config.AimbotSmoothness}");  // Displaying the FOV value in the status message
        }

        private void guna2TrackBar1_Scroll(object sender, ScrollEventArgs e)
        {
            var distance = guna2TrackBar1.Value;
            rangeaim.Text = $"{distance}";
            Config.AimBotMaxDistance = distance;
            UpdateStatus(true, $"Aim Range : {Config.AimBotMaxDistance}");  // Displaying the FOV value in the status message
        }

        private void guna2CustomCheckBox29_Click(object sender, EventArgs e)
        {
            Config.Notif();
            Config.MagnetPull = guna2CustomCheckBox29.Checked;
            if (guna2CustomCheckBox29.Checked)
                EnsureFeatureThread("MagnetPull", MagnetPull.Start);
            status.Text = guna2CustomCheckBox29.Checked
                ? "Magnet Pull Enabled!"
                : "Magnet Pull Disabled!";
        }

        private void guna2CustomCheckBox20_Click(object sender, EventArgs e)
        {
            Config.Notif();
            Config.EnemyPullEnabled1 = guna2CustomCheckBox20.Checked;
            if (guna2CustomCheckBox20.Checked)
                EnsureFeatureThread("EnemyPull360", EnemyPull360.Start);
            status.Text = guna2CustomCheckBox20.Checked
                ? "EnemyPull Enabled!"
                : "EnemyPull Disabled!";
        }

        private void guna2CustomCheckBox19_Click(object sender, EventArgs e)
        {
            Config.success();
            Config.SilentAim = guna2CustomCheckBox19.Checked;
            if (guna2CustomCheckBox19.Checked)
                EnsureFeatureThread("SilentAim", SilentAim.Work);
            status.Text = guna2CustomCheckBox19.Checked

                ? "Silent Aim Max Enabled!"
                : "Silent Aim Max Disabled!";
        }

        private void guna2CustomCheckBox11_Click(object sender, EventArgs e)
        {
            Config.success();
            Config.AimBot = guna2CustomCheckBox11.Checked;
            if (guna2CustomCheckBox11.Checked)
                EnsureFeatureThread("Aimbot", Aimbot.Work);
            status.Text = guna2CustomCheckBox11.Checked
                ? "Aimbot Rage Enabled!"
                : "Aimbot Rage Disabled!";
        }

        private void guna2CustomCheckBox27_Click(object sender, EventArgs e)
        {
            Config.Notif();
            Config.IgnoreKnocked = guna2CustomCheckBox27.Checked;
            status.Text = "Ignore Knock Done !";
        }

        private void guna2CustomCheckBox9_Click(object sender, EventArgs e)
        {
            Config.Notif();
            Config.NoRecoil = guna2CustomCheckBox9.Checked;
            status.Text = "No Recoil Done !";
        }

        private void guna2CustomCheckBox18_Click(object sender, EventArgs e)
        {
            Config.Notif();
            Config.AimFov = guna2CustomCheckBox18.Checked;
            Config.Aimfovc = guna2CustomCheckBox18.Checked;
            status.Text = "Aim FOV Done !";
        }

        private void guna2CustomCheckBox31_Click(object sender, EventArgs e)
        {
            Config.Notif();
            if (guna2CustomCheckBox31.Checked)
            {
                Config.ESPLine = true;
                Config.EspUp = true;
                status.Text = "ESP LINE ENABLED";
            }
            else
            {
                Config.ESPLine = false;
                Config.EspUp = false;
                status.Text = "ESP LINE DISABLED";
            }
        }

        private void guna2CustomCheckBox32_Click_1(object sender, EventArgs e)
        {
            Config.Notif();
            if (guna2CustomCheckBox32.Checked)
            {
                Config.ESPBox = true;
                status.Text = "ESP BOX ENABLED";
                guna2Separator4.Visible = true;
                guna2Separator5.Visible = true;
                guna2vSeparator1.Visible = true;
                guna2vSeparator2.Visible = true;
            }
            else
            {
                Config.ESPBox = false;
                status.Text = "ESP BOX DISABLED";
                guna2Separator4.Visible = false;
                guna2Separator5.Visible = false;
                guna2vSeparator2.Visible = false;
                guna2vSeparator1.Visible = false;
            }
        }

        private void guna2CustomCheckBox25_Click(object sender, EventArgs e)
        {
            Config.Notif();
            Config.ESPFillBox = guna2CustomCheckBox25.Checked;
            UpdateStatus(guna2CustomCheckBox25.Checked, "ESP Fill Box");
        }

        private void guna2CustomCheckBox23_Click(object sender, EventArgs e)
        {
            Config.Notif();
            if (guna2CustomCheckBox23.Checked)
            {
                Config.ESPName = true;
                Config.ESPDistance = true;
                //  Config.SystemSpec = true;
                Config.ESPHealth = true;
                status.Text = "ESP INFO ENABLED";
                label6.Visible = true;
                label6.Visible = true;
                guna2CustomGradientPanel2.Visible = true;
            }
            else
            {
                Config.ESPName = false;
                Config.ESPDistance = false;
                // Config.SystemSpec = false;
                Config.ESPHealth = false;
                status.Text = "ESP INFO DISABLED";
                label6.Visible = false;
                label6.Visible = false;
                guna2CustomGradientPanel2.Visible = false;
            }
        }

        private void guna2CustomCheckBox24_Click(object sender, EventArgs e)
        {
            Config.Notif();
            if (guna2CustomCheckBox24.Checked)
            {
                Config.minimap = true;
                status.Text = "ESP MINIMAP ENABLED";
            }
            else
            {
                Config.minimap = false;
                status.Text = "ESP MINIMAP DISABLED";
            }
        }

        private void guna2CustomCheckBox33_Click(object sender, EventArgs e)
        {
            Config.Notif();
            if (guna2CustomCheckBox33.Checked)
            {
                Config.ESPSkeleton = true;
                status.Text = "ESP SKELETON ENABLED";
            }
            else
            {
                Config.ESPSkeleton = false;
                status.Text = "ESP SKELETON DISABLED";
            }
        }

        private void guna2ToggleSwitch18_CheckedChanged(object sender, EventArgs e)
        {
            Config.Notif();
            Config.FastReload = guna2ToggleSwitch18.Checked;
            status.Text = guna2ToggleSwitch18.Checked ? "Fast Reload Enabled ! " : "Fast Reload Disabled !";
        }

        private async void guna2Button37_Click(object sender, EventArgs e)
        {
            Config.success();
            UpdateStatuss("Refreshing ESP...", Color.Yellow);

            try
            {
                EspController.Stop();          // ESP OFF
                await Task.Delay(300);         // short cooldown
                await EspController.StartAsync(); // ESP ON again

                UpdateStatuss("ESP refreshed!", Color.Lime);
            }
            catch
            {
                UpdateStatuss("ESP refresh failed!", Color.Red);
            }
        }

        private void guna2Button36_Click(object sender, EventArgs e)
        {
            Config.success();
            string url = "https://discord.gg/cheatexe";

            try
            {
                System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo
                {
                    FileName = url,
                    UseShellExecute = true
                });

                // ✅ SUCCESS FEEDBACK
                info.Show("Discord opened successfully!");
                // ya agar status label hai:
                // UpdateStatuss("Discord opened successfully!", Color.Lime);
            }
            catch (Exception ex)
            {
                // ❌ FAILED FEEDBACK
                info.Show("Failed to open link!");
                // UpdateStatuss("Failed to open Discord!", Color.Red);
                // optional debug:
                // MessageBox.Show(ex.Message);
            }
        }

        private void guna2CustomCheckBox40_Click(object sender, EventArgs e)
        {
            Config.success();
            this.TopMost = guna2CustomCheckBox40.Checked;

            if (guna2CustomCheckBox40.Checked)
            {
                //  info.Show("Pin on Top enabled");
                UpdateStatuss("Pin on Top enabled", Color.Lime);
            }
            else
            {
                // info.Show("Pin on Top disabled");
                UpdateStatuss("Pin on Top disabled", Color.Red);
            }
        }




        private void guna2Button27_Click(object sender, EventArgs e)
        {
            Config.Notif();
            var picker = new ColorDialog();
            var result = picker.ShowDialog();
            if (result == DialogResult.OK)
            {
                guna2Button27.FillColor = picker.Color;
                Config.ESPLineColor = picker.Color;
            }
        }

        private void guna2Button26_Click(object sender, EventArgs e)
        {
            Config.Notif();
            var picker = new ColorDialog();
            var result = picker.ShowDialog();

            if (result == DialogResult.OK)
            {
                guna2Button26.FillColor = picker.Color;
                Config.ESPBoxColor = picker.Color;
            }
        }

        private void guna2Button28_Click(object sender, EventArgs e)
        {
            Config.Notif();
            var picker = new ColorDialog();
            var result = picker.ShowDialog();
            if (result == DialogResult.OK)
            {
                guna2Button28.FillColor = picker.Color;
                Config.ESPSkeletonColor = picker.Color;
            }
        }

        private void guna2CustomCheckBox41_Click(object sender, EventArgs e)
        {
            Map_Teleport.EnableFlyHook(guna2CustomCheckBox41.Checked);
            status.Text = guna2CustomCheckBox41.Checked
                ? "Flyhack Enabled!"
                : "Flyhack Disabled!";
        }

        private void guna2CustomCheckBox42_Click(object sender, EventArgs e)
        {
            Config.Notif();
            Config.timerEnabled = guna2CustomCheckBox36.Checked;
            status.Text = guna2CustomCheckBox36.Checked
                ? "Invalid Timer Enabled!"
                : "Invalid Timer Disabled!";
        }

        private void guna2CustomCheckBox39_Click(object sender, EventArgs e)
        {
            Config.success();
            // Agar checked hai toh sound ON hona chahiye
            if (guna2CustomCheckBox39.Checked)
            {
                Config.sound = true;
                status.Text = "Notification Sound Enabled";
            }
            else
            {
                Config.sound = false;
                status.Text = "Notification Sound Muted";
            }
        }

        private void guna2CustomCheckBox36_Click(object sender, EventArgs e)
        {
            Config.Notif();
            Config.timerEnabled = guna2CustomCheckBox36.Checked;
            status.Text = guna2CustomCheckBox36.Checked
                ? "Invalid Timer Enabled!"
                : "Invalid Timer Disabled!";
        }

        private void guna2CustomCheckBox30_Click(object sender, EventArgs e)
        {

        }

        private void guna2CustomCheckBox22_Click_1(object sender, EventArgs e)
        {
            Config.Notif();

            Config.RGB = guna2CustomCheckBox22.Checked;

            if (Config.RGB)
            {
                status.Text = "RGB ENABLED";
                status.ForeColor = Color.Lime;

                guna2CustomCheckBox22.ForeColor = Color.White;
            }
            else
            {
                status.Text = "RGB DISABLED";
                status.ForeColor = Color.Red;

                guna2CustomCheckBox22.ForeColor = Color.Silver;
            }
        }


        private void guna2Button24_Click(object sender, EventArgs e)
        {

        }

        //private void guna2CustomCheckBox1_Click(object sender, EventArgs e)
        //{
        //    Config.Notif();
        //    Config.ShakeKill = guna2CustomCheckBox1.Checked;
        //    status.Text = guna2CustomCheckBox1.Checked
        //        ? "Rapid Fire Enabled!"
        //        : "Rapid Fire Disabled!";

        //}

        private void guna2ControlBox1_Click(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        private void aimfovtxt_Click(object sender, EventArgs e)
        {

        }


        private void password_TextChanged(object sender, EventArgs e)
        {

        }

        public void DisableSpeedAndFastFireFromLobby()
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(DisableSpeedAndFastFireFromLobby));
                return;
            }

            try
            {
                guna2ToggleSwitch10.Checked = false;
                guna2ToggleSwitch17.Checked = false;
                status.Text = "Lobby Auto-Reset Done";
            }
            catch { }
        }

        private void login_Paint(object sender, PaintEventArgs e)
        {

        }
    }
}