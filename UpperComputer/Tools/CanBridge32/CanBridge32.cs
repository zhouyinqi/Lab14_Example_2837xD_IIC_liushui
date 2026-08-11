using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;

internal static class CanBridge32
{
    private const UInt32 StatusOk = 1U;
    private const UInt32 RequestId = 0x321U;
    private const UInt32 ResponseId = 0x001U;

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    private struct VciInitConfig
    {
        public UInt32 AcceptanceCode;
        public UInt32 AcceptanceMask;
        public UInt32 Reserved;
        public Byte Filter;
        public Byte Timing0;
        public Byte Timing1;
        public Byte Mode;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    private struct VciCanObject
    {
        public UInt32 Id;
        public UInt32 TimeStamp;
        public Byte TimeFlag;
        public Byte SendType;
        public Byte RemoteFlag;
        public Byte ExternFlag;
        public Byte DataLength;
        public Byte Data0;
        public Byte Data1;
        public Byte Data2;
        public Byte Data3;
        public Byte Data4;
        public Byte Data5;
        public Byte Data6;
        public Byte Data7;
        public Byte Reserved0;
        public Byte Reserved1;
        public Byte Reserved2;

        public Boolean HasTestData()
        {
            return DataLength == 4U &&
                   Data0 == 0xA5U && Data1 == 0x5AU &&
                   Data2 == 0xA5U && Data3 == 0x5AU;
        }
    }

    private static class NativeMethods
    {
        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        internal static extern Boolean SetDllDirectory(String pathName);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
        internal static extern IntPtr LoadLibrary(String fileName);

        [DllImport("kernel32.dll", SetLastError = true)]
        internal static extern Boolean FreeLibrary(IntPtr module);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Ansi)]
        internal static extern IntPtr GetProcAddress(IntPtr module, String procedureName);
    }

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate UInt32 VciOpenDevice(UInt32 deviceType, UInt32 deviceIndex, UInt32 reserved);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate UInt32 VciCloseDevice(UInt32 deviceType, UInt32 deviceIndex);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate UInt32 VciInitCan(UInt32 deviceType,
                                       UInt32 deviceIndex,
                                       UInt32 canIndex,
                                       ref VciInitConfig initConfig);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate UInt32 VciStartCan(UInt32 deviceType, UInt32 deviceIndex, UInt32 canIndex);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate UInt32 VciResetCan(UInt32 deviceType, UInt32 deviceIndex, UInt32 canIndex);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate UInt32 VciClearBuffer(UInt32 deviceType, UInt32 deviceIndex, UInt32 canIndex);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate UInt32 VciTransmit(UInt32 deviceType,
                                        UInt32 deviceIndex,
                                        UInt32 canIndex,
                                        ref VciCanObject send,
                                        UInt32 length);

    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    private delegate UInt32 VciReceive(UInt32 deviceType,
                                       UInt32 deviceIndex,
                                       UInt32 canIndex,
                                       [In, Out] VciCanObject[] receive,
                                       UInt32 length,
                                       Int32 waitTime);

    private sealed class ControlCanApi : IDisposable
    {
        private IntPtr m_module;

        public VciOpenDevice OpenDevice;
        public VciCloseDevice CloseDevice;
        public VciInitCan InitCan;
        public VciStartCan StartCan;
        public VciResetCan ResetCan;
        public VciClearBuffer ClearBuffer;
        public VciTransmit Transmit;
        public VciReceive Receive;

        public static ControlCanApi Load(String libraryPath)
        {
            ControlCanApi api = new ControlCanApi();
            api.m_module = NativeMethods.LoadLibrary(libraryPath);
            if (api.m_module == IntPtr.Zero)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error(),
                                         "Cannot load " + libraryPath);
            }

            try
            {
                api.OpenDevice = Resolve<VciOpenDevice>(api.m_module, "VCI_OpenDevice");
                api.CloseDevice = Resolve<VciCloseDevice>(api.m_module, "VCI_CloseDevice");
                api.InitCan = Resolve<VciInitCan>(api.m_module, "VCI_InitCAN");
                api.StartCan = Resolve<VciStartCan>(api.m_module, "VCI_StartCAN");
                api.ResetCan = Resolve<VciResetCan>(api.m_module, "VCI_ResetCAN");
                api.ClearBuffer = Resolve<VciClearBuffer>(api.m_module, "VCI_ClearBuffer");
                api.Transmit = Resolve<VciTransmit>(api.m_module, "VCI_Transmit");
                api.Receive = Resolve<VciReceive>(api.m_module, "VCI_Receive");
                return api;
            }
            catch
            {
                api.Dispose();
                throw;
            }
        }

        public void Dispose()
        {
            if (m_module != IntPtr.Zero)
            {
                NativeMethods.FreeLibrary(m_module);
                m_module = IntPtr.Zero;
            }
        }

        private static T Resolve<T>(IntPtr module, String procedureName) where T : class
        {
            IntPtr procedure = NativeMethods.GetProcAddress(module, procedureName);
            if (procedure == IntPtr.Zero)
            {
                throw new EntryPointNotFoundException(procedureName);
            }
            return Marshal.GetDelegateForFunctionPointer(procedure, typeof(T)) as T;
        }
    }

    private static Int32 Main(String[] args)
    {
        Dictionary<String, String> options = ParseOptions(args);
        String mode = GetOption(options, "mode");
        String dllPath = GetOption(options, "dll");
        UInt32 deviceType;
        UInt32 deviceIndex;
        UInt32 channelIndex;
        Int32 timeoutMs;

        if ((mode != "probe" && mode != "test") || !File.Exists(dllPath) ||
            !UInt32.TryParse(GetOption(options, "type"), out deviceType) ||
            !UInt32.TryParse(GetOption(options, "index"), out deviceIndex) ||
            !UInt32.TryParse(GetOption(options, "channel"), out channelIndex) ||
            !Int32.TryParse(GetOption(options, "timeout-ms"), out timeoutMs) ||
            timeoutMs <= 0)
        {
            Fail("ARGUMENT", "Usage: --mode probe|test --dll path --type n --index n --channel n --timeout-ms n");
            return 2;
        }

        String apiLibraryPath = ResolveApiLibrary(dllPath);
        String libraryDirectory = Path.GetDirectoryName(Path.GetFullPath(apiLibraryPath));
        Environment.CurrentDirectory = libraryDirectory;
        NativeMethods.SetDllDirectory(libraryDirectory);

        Boolean deviceOpened = false;
        Boolean channelStarted = false;
        ControlCanApi api = null;
        try
        {
            api = ControlCanApi.Load(apiLibraryPath);
            UInt32 result = api.OpenDevice(deviceType, deviceIndex, 0U);
            if (result != StatusOk)
            {
                Fail("OPEN", "VCI_OpenDevice returned " + result);
                return 3;
            }
            deviceOpened = true;

            if (mode == "probe")
            {
                api.CloseDevice(deviceType, deviceIndex);
                deviceOpened = false;
                Console.WriteLine("RESULT=READY");
                return 0;
            }

            VciInitConfig config = new VciInitConfig();
            config.AcceptanceCode = 0U;
            config.AcceptanceMask = 0xFFFFFFFFU;
            config.Reserved = 0U;
            config.Filter = 1;
            config.Timing0 = 0x00;
            config.Timing1 = 0x1C;
            config.Mode = 0;

            if (api.InitCan(deviceType, deviceIndex, channelIndex, ref config) != StatusOk)
            {
                Fail("INIT", "VCI_InitCAN failed");
                return 4;
            }
            if (api.ClearBuffer(deviceType, deviceIndex, channelIndex) != StatusOk)
            {
                Fail("CLEAR", "VCI_ClearBuffer failed");
                return 5;
            }
            if (api.StartCan(deviceType, deviceIndex, channelIndex) != StatusOk)
            {
                Fail("START", "VCI_StartCAN failed");
                return 6;
            }
            channelStarted = true;
            Thread.Sleep(100);

            VciCanObject request = new VciCanObject();
            request.Id = RequestId;
            request.SendType = 0;
            request.RemoteFlag = 0;
            request.ExternFlag = 0;
            request.DataLength = 4;
            request.Data0 = 0xA5;
            request.Data1 = 0x5A;
            request.Data2 = 0xA5;
            request.Data3 = 0x5A;
            UInt32 transmitResult = 0U;
            Int32 transmitAttempt;
            for (transmitAttempt = 1; transmitAttempt <= 3; ++transmitAttempt)
            {
                transmitResult = api.Transmit(deviceType,
                                              deviceIndex,
                                              channelIndex,
                                              ref request,
                                              1U);
                if (transmitResult == StatusOk)
                {
                    break;
                }
                Thread.Sleep(100);
            }
            if (transmitResult != StatusOk)
            {
                Fail("TRANSMIT",
                     "VCI_Transmit returned " + transmitResult +
                     " after " + (transmitAttempt - 1) + " attempts; check CANH/CANL, termination and bus ACK");
                return 7;
            }

            DateTime deadline = DateTime.UtcNow.AddMilliseconds(timeoutMs);
            VciCanObject[] receive = new VciCanObject[16];
            while (DateTime.UtcNow < deadline)
            {
                UInt32 receiveCount = api.Receive(deviceType,
                                                  deviceIndex,
                                                  channelIndex,
                                                  receive,
                                                  (UInt32)receive.Length,
                                                  0);
                if (receiveCount == 0xFFFFFFFFU)
                {
                    Fail("RECEIVE", "VCI_Receive failed");
                    return 8;
                }
                for (UInt32 index = 0U; index < receiveCount && index < receive.Length; ++index)
                {
                    VciCanObject frame = receive[index];
                    if (frame.ExternFlag == 0U && frame.RemoteFlag == 0U &&
                        frame.Id == ResponseId && frame.HasTestData())
                    {
                        Console.WriteLine("RESULT=PASS");
                        return 0;
                    }
                }
                Thread.Sleep(20);
            }

            Fail("TIMEOUT", "No response frame 0x001 A5 5A A5 5A");
            return 9;
        }
        catch (BadImageFormatException exception)
        {
            Fail("DLL_ARCHITECTURE", exception.Message);
            return 10;
        }
        catch (DllNotFoundException exception)
        {
            Fail("DLL_LOAD", exception.Message);
            return 11;
        }
        catch (EntryPointNotFoundException exception)
        {
            Fail("DLL_EXPORT", exception.Message);
            return 12;
        }
        catch (Exception exception)
        {
            Fail("EXCEPTION", exception.Message);
            return 13;
        }
        finally
        {
            if (channelStarted)
            {
                api.ResetCan(deviceType, deviceIndex, channelIndex);
            }
            if (deviceOpened)
            {
                api.CloseDevice(deviceType, deviceIndex);
            }
            if (api != null)
            {
                api.Dispose();
            }
        }
    }

    private static Dictionary<String, String> ParseOptions(String[] args)
    {
        Dictionary<String, String> options = new Dictionary<String, String>(StringComparer.OrdinalIgnoreCase);
        for (Int32 index = 0; index + 1 < args.Length; index += 2)
        {
            if (args[index].StartsWith("--", StringComparison.Ordinal))
            {
                options[args[index].Substring(2)] = args[index + 1];
            }
        }
        return options;
    }

    private static String ResolveApiLibrary(String selectedLibrary)
    {
        String selectedName = Path.GetFileName(selectedLibrary);
        String selectedDirectory = Path.GetDirectoryName(Path.GetFullPath(selectedLibrary));
        String usbCanLibrary = Path.Combine(selectedDirectory, "kerneldlls", "USBCAN.dll");

        if (String.Equals(selectedName, "ControlCAN.dll", StringComparison.OrdinalIgnoreCase) &&
            File.Exists(usbCanLibrary))
        {
            return usbCanLibrary;
        }
        return selectedLibrary;
    }

    private static String GetOption(Dictionary<String, String> options, String name)
    {
        String value;
        return options.TryGetValue(name, out value) ? value : String.Empty;
    }

    private static void Fail(String code, String message)
    {
        Console.WriteLine("RESULT=FAIL;CODE=" + code + ";MESSAGE=" +
                          message.Replace(';', ',').Replace('\r', ' ').Replace('\n', ' '));
    }
}
