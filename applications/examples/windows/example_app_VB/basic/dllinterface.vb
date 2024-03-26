Imports System.Runtime.InteropServices

Public Class spectrInterface
    <DllImport("spectrlib_core_shared.dll", CallingConvention:=CallingConvention.Cdecl)>
    Public Shared Function getDevicesCount() As UInteger
    End Function
    <DllImport("spectrlib_core_shared.dll", CallingConvention:=CallingConvention.Cdecl)>
    Public Shared Function disconnectDeviceContext(ByRef deviceContextPtr As UIntPtr) As Integer
    End Function
    <DllImport("spectrlib_core_shared.dll", CallingConvention:=CallingConvention.Cdecl)>
    Public Shared Function connectToDeviceByIndex(deviceIndex As UInteger, ByRef deviceContextPtr As UIntPtr) As Integer
    End Function
    <DllImport("spectrlib_core_shared.dll", CallingConvention:=CallingConvention.Cdecl)>
    Public Shared Function setAcquisitionParameters(numOfScans As UShort, numOfBlankScans As UShort, scanMode As Byte, timeOfExposure As UInteger, ByRef deviceContextPtr As UIntPtr) As Integer
    End Function
    <DllImport("spectrlib_core_shared.dll", CallingConvention:=CallingConvention.Cdecl)>
    Public Shared Function getStatus(ByRef statusFlags As Byte, ByRef framesInMemory As UShort, ByRef deviceContextPtr As UIntPtr) As Integer
    End Function
    <DllImport("spectrlib_core_shared.dll", CallingConvention:=CallingConvention.Cdecl)>
    Public Shared Function getFrameFormat(ByRef numOfStartElement As UShort, ByRef numOfEndElement As UShort, ByRef reductionMode As Byte, ByRef numOfPixelsInFrame As UShort, ByRef deviceContextPtr As UIntPtr) As Integer
    End Function
    <DllImport("spectrlib_core_shared.dll", CallingConvention:=CallingConvention.Cdecl)>
    Public Shared Function setFrameFormat(numOfStartElement As UShort, numOfEndElement As UShort, reductionMode As Byte, ByRef numOfPixelsInFrame As UShort, ByRef deviceContextPtr As UIntPtr) As Integer
    End Function
    <DllImport("spectrlib_core_shared.dll", CallingConvention:=CallingConvention.Cdecl)>
    Public Shared Function triggerAcquisition(ByRef deviceContextPtr As UIntPtr) As Integer
    End Function
    <DllImport("spectrlib_core_shared.dll", CallingConvention:=CallingConvention.Cdecl)>
    Public Shared Function clearMemory(ByRef deviceContextPtr As UIntPtr) As Integer
    End Function
    <DllImport("spectrlib_core_shared.dll", CallingConvention:=CallingConvention.Cdecl)>
    Public Shared Function getFrame(framePixelsBuffer() As UShort, numOfFrame As UShort, ByRef deviceContextPtr As UIntPtr) As Integer
    End Function
    <DllImport("spectrlib_core_shared.dll", CallingConvention:=CallingConvention.Cdecl)>
    Public Shared Function getAcquisitionParameters(ByRef numOfScans As UShort, ByRef numOfBlankScans As UShort, ByRef scanMode As Byte, ByRef timeOfExposure As UInteger, ByRef deviceContextPtr As UIntPtr) As Integer
    End Function
    <DllImport("spectrlib_core_shared.dll", CallingConvention:=CallingConvention.Cdecl)>
    Public Shared Function eraseFlash(ByRef deviceContextPtr As UIntPtr) As Integer
    End Function
    <DllImport("spectrlib_core_shared.dll", CallingConvention:=CallingConvention.Cdecl)>
    Public Shared Function readFlash(buffer() As Byte, absoluteOffset As UInteger, bytesToRead As UInteger, ByRef deviceContextPtr As UIntPtr) As Integer
    End Function
    <DllImport("spectrlib_core_shared.dll", CallingConvention:=CallingConvention.Cdecl)>
    Public Shared Function writeFlash(buffer() As Byte, absoluteOffset As UInteger, bytesToRead As UInteger, ByRef deviceContextPtr As UIntPtr) As Integer
    End Function

End Class
