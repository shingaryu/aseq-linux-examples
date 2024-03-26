Imports System

Public Class Form1

    Private Const ExposureMultiplier As UInteger = 10000
    Private Const ReadFlashOffset As UInteger = 0
    Private Const ReadFlashBytes As UInteger = 50
    Private Const WriteFlashOffset As UInteger = 0
    Private Const WriteFlashBytes As UInteger = 50

    Private _devicesCount As UInteger
    Private _connectionErrors() As Integer
    Private _numOfPixelsInFrame() As UShort
    Private _deviceBuffers() As DeviceBuffers
    Private _deviceContexts() As UIntPtr


    Const absoluteOffset As UInteger = 0
    Const bytesToWrite As UInteger = 1000
    Const bytesToRead As UInteger = 1000


    Private Sub ConnectButton_Click(sender As Object, e As EventArgs) Handles ConnectButton.Click

        If _devicesCount > 0 Then
            For index As Byte = 0 To _devicesCount - 1
                spectrInterface.disconnectDeviceContext(_deviceContexts(index))
            Next
        End If

        _devicesCount = spectrInterface.getDevicesCount()

        If _devicesCount = 0 Then
            Log.AppendText("No devices found!" + vbCrLf)
            Return
        End If

        ReDim _deviceContexts(_devicesCount)
        ReDim _deviceBuffers(_devicesCount)
        ReDim _connectionErrors(_devicesCount)
        ReDim _numOfPixelsInFrame(_devicesCount)

        For index As Integer = 0 To _devicesCount - 1
            _deviceBuffers(index) = New DeviceBuffers()
        Next

        For index As Byte = 0 To _devicesCount - 1
            _connectionErrors(index) = spectrInterface.connectToDeviceByIndex(index, _deviceContexts(index))
            If _connectionErrors(index) = 0 Then
                Log.AppendText("Connecting device " + index.ToString() + " .... success" + vbCrLf)
            Else
                Log.AppendText("Connecting device " + index.ToString() + " .... failed, error code: " + _connectionErrors(index).ToString + vbCrLf)
            End If
        Next

        StatusButton.Enabled = True
        GetParametersButton.Enabled = True
        SetParametersButton.Enabled = True
        TriggerButton.Enabled = False
        GetFrameButton.Enabled = False
        EraseFlashButton.Enabled = True
        ReadFlashButton.Enabled = True
        WriteFlashButton.Enabled = True
        ClearMemoryButton.Enabled = True

    End Sub


    Private Sub StatusButton_Click(sender As Object, e As EventArgs) Handles StatusButton.Click
        Dim framesInMemory As UShort
        Dim status As Byte
        Dim result As Integer

        For index As Byte = 0 To _devicesCount - 1
            If _connectionErrors(index) = 0 Then
                result = spectrInterface.getStatus(status, framesInMemory, _deviceContexts(index))
                If (result = 0) Then
                    Log.AppendText("Status flags for device " + index.ToString() + "= " + status.ToString + " frames in memory: " + framesInMemory.ToString + vbCrLf)
                Else
                    Log.AppendText("Can't get status for device " + index.ToString() + ". Error code: " + result.ToString + vbCrLf)
                End If
            End If
        Next
    End Sub

    Private Sub TriggerAcquisitionButton_Click(sender As Object, e As EventArgs) Handles TriggerButton.Click
        Dim result As Integer

        For index As Byte = 0 To _devicesCount - 1
            If _connectionErrors(index) = 0 Then
                result = spectrInterface.triggerAcquisition(_deviceContexts(index))
                If (result = 0) Then
                    Log.AppendText("triggerAcquisition() for device " + index.ToString() + "... success" + vbCrLf)

                    GetFrameButton.Enabled = True
                Else
                    Log.AppendText("triggerAcquisition() for device " + index.ToString() + " failed. Error code: " + result.ToString + vbCrLf)
                End If
            End If
        Next

        GetFrameButton.Enabled = True
    End Sub

    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load        
        ConnectButton.Enabled = True
        StatusButton.Enabled = False
        GetParametersButton.Enabled = False
        SetParametersButton.Enabled = False
        TriggerButton.Enabled = False
        GetFrameButton.Enabled = False
        EraseFlashButton.Enabled = False
        ReadFlashButton.Enabled = False
        WriteFlashButton.Enabled = False
        ClearMemoryButton.Enabled = False
    End Sub

    Private Sub GetParametersButton_Click(sender As Object, e As EventArgs) Handles GetParametersButton.Click
        Dim numScans As UShort
        Dim numBlankScans As UShort
        Dim scanMode As Byte
        Dim exposure As UInteger
        Dim startElement As UShort
        Dim endElement As UShort
        Dim reductionMode As Byte
        Dim result As Integer

        For index As Byte = 0 To _devicesCount - 1
            If _connectionErrors(index) = 0 Then
                result = spectrInterface.getAcquisitionParameters(numScans, numBlankScans, scanMode, exposure, _deviceContexts(index))
                If (result = 0) Then
                    Log.AppendText("getAcquisitionParameters() for device " + index.ToString() + ": numScans: " + numScans.ToString + ", numBlankScans: " + numBlankScans.ToString + ", scanMode: " + scanMode.ToString + ", exposure: " + exposure.ToString() + vbCrLf)
                Else
                    Log.AppendText("getAcquisitionParameters() for device " + index.ToString() + " failed. Error code: " + result.ToString + vbCrLf)
                End If
            End If
        Next

        For index As Byte = 0 To _devicesCount - 1
            If _connectionErrors(index) = 0 Then
                result = spectrInterface.getFrameFormat(startElement, endElement, reductionMode, _numOfPixelsInFrame(index), _deviceContexts(index))
                If (result = 0) Then
                    Log.AppendText("getFrameFormat() for device " + index.ToString() + ": startElement:  " + startElement.ToString + " endElement: " + endElement.ToString + " reductionMode: " + reductionMode.ToString + " pixelsInFrame: " + _numOfPixelsInFrame(index).ToString + vbCrLf)

                    TriggerButton.Enabled = True
                Else
                    Log.AppendText("getFrameFormat() for device " + index.ToString() + " failed. Error code: " + result.ToString + vbCrLf)
                End If
            End If
        Next
    End Sub

    Private Sub SetParametersButton_Click(sender As Object, e As EventArgs) Handles SetParametersButton.Click
        Const scanMode As Byte = 3
        Const numOfBlankScans As UShort = 0
        Const numOfScans As UShort = 1
        Dim exposure As UInteger

        Dim startElement As UShort
        Dim endElement As UShort
        Dim reductionMode As Byte

        Dim result As Integer

        For index As Byte = 0 To _devicesCount - 1
            If _connectionErrors(index) = 0 Then
                exposure = ExposureMultiplier * (index + 1)
                result = spectrInterface.setAcquisitionParameters(numOfScans, numOfBlankScans, scanMode, exposure, _deviceContexts(index))
                If (result = 0) Then
                    Log.AppendText("setAcquisitionParameters() for device " + index.ToString() + ": numOfScans:  " + numOfScans.ToString + " numOfBlankScans: " + numOfBlankScans.ToString + " scanMode: " + scanMode.ToString + " exposure: " + exposure.ToString + vbCrLf)
                Else
                    Log.AppendText("setAcquisitionParameters() for device " + index.ToString() + " failed. Error code: " + result.ToString + vbCrLf)
                End If
            End If
        Next

        For index As Byte = 0 To _devicesCount - 1
            If _connectionErrors(index) = 0 Then
                result = spectrInterface.getFrameFormat(startElement, endElement, reductionMode, _numOfPixelsInFrame(index), _deviceContexts(index))
                If (result = 0) Then
                    Log.AppendText("getFrameFormat() for device " + index.ToString() + ": startElement:  " + startElement.ToString + " endElement: " + endElement.ToString + " reductionMode: " + reductionMode.ToString + " pixelsInFrame: " + _numOfPixelsInFrame(index).ToString + vbCrLf)

                    TriggerButton.Enabled = True
                Else
                    Log.AppendText("getFrameFormat() for device " + index.ToString() + " failed. Error code: " + result.ToString + vbCrLf)
                End If
            End If
        Next
    End Sub

    Private Sub ClearMemoryButton_Click(sender As Object, e As EventArgs) Handles ClearMemoryButton.Click
        Dim result As Integer

        For index As Byte = 0 To _devicesCount - 1
            If _connectionErrors(index) = 0 Then
                result = spectrInterface.clearMemory(_deviceContexts(index))
                If (result = 0) Then
                    Log.AppendText("clearMemory() for device " + index.ToString() + ": success" + vbCrLf)
                Else
                    Log.AppendText("clearMemory() for device " + index.ToString() + " failed. Error code: " + result.ToString + vbCrLf)
                End If
            End If
        Next

    End Sub

    Private Sub GetFrameButton_Click(sender As Object, e As EventArgs) Handles GetFrameButton.Click
        Dim result As Integer
        Dim bufferString As String

        For index As Byte = 0 To _devicesCount - 1
            If _connectionErrors(index) = 0 Then
                If _numOfPixelsInFrame(index) Then
                    ReDim _deviceBuffers(index).frameBuffer(_numOfPixelsInFrame(index))
                    result = spectrInterface.getFrame(_deviceBuffers(index).frameBuffer, &HFFFF, _deviceContexts(index))    '' &HFFFF = 0xFFFF = 65535

                    If (result = 0) Then
                        bufferString = "getFrame() for device " + index.ToString() + ", frame: "

                        For i As Integer = 0 To _numOfPixelsInFrame(index)
                            bufferString += _deviceBuffers(index).frameBuffer(i).ToString + " "
                        Next

                        Log.AppendText(bufferString + vbCrLf)
                    Else
                        Log.AppendText("getFrame() for device " + index.ToString() + " failed. Error code: " + result.ToString + vbCrLf)
                    End If
                Else
                    Log.AppendText("getFrame() for device " + index.ToString() + ": start failed. Zero number of pixels in frame!" + vbCrLf)
                End If
            End If
        Next
    End Sub

    Private Sub eraseFlashButton_Click(sender As Object, e As EventArgs) Handles EraseFlashButton.Click
        Dim result As Integer

        For index As Byte = 0 To _devicesCount - 1
            If _connectionErrors(index) = 0 Then
                result = spectrInterface.eraseFlash(_deviceContexts(index))
                If (result = 0) Then
                    Log.AppendText("eraseFlash() for device " + index.ToString() + ": success" + vbCrLf)
                Else
                    Log.AppendText("eraseFlash() for device " + index.ToString() + " failed. Error code: " + result.ToString + vbCrLf)
                End If
            End If
        Next
    End Sub

    Private Sub readFlashButton_Click(sender As Object, e As EventArgs) Handles ReadFlashButton.Click
        Const absoluteOffset As UInteger = 0
        Const bytesToRead As UInteger = 1000

        Dim result As Integer
        Dim bufferString As String

        For index As Byte = 0 To _devicesCount - 1
            If _connectionErrors(index) = 0 Then
                ReDim _deviceBuffers(index).flashBuffer(bytesToRead)
                result = spectrInterface.readFlash(_deviceBuffers(index).flashBuffer, absoluteOffset, bytesToRead, _deviceContexts(index))
                If (result = 0) Then
                    bufferString = "readFlash() for device " + index.ToString() + ", frame: "

                    For i As Integer = 0 To ReadFlashBytes
                        bufferString += _deviceBuffers(index).flashBuffer(i).ToString + " "
                    Next

                    Log.AppendText(bufferString + vbCrLf)
                Else
                    Log.AppendText("readFlash() for device " + index.ToString() + " start failed. Error code: " + result.ToString + vbCrLf)
                End If
            End If
        Next
    End Sub

    Private Sub writeFlashButton_Click(sender As Object, e As EventArgs) Handles WriteFlashButton.Click
        Dim result As Integer

        For index As Byte = 0 To _devicesCount - 1
            If _connectionErrors(index) = 0 Then
                ReDim _deviceBuffers(index).flashBuffer(bytesToWrite)
                For i As Integer = 0 To bytesToWrite
                    _deviceBuffers(index).flashBuffer(i) = CInt(134)
                Next

                result = spectrInterface.writeFlash(_deviceBuffers(index).flashBuffer, absoluteOffset, bytesToWrite, _deviceContexts(index))

                If (result = 0) Then
                    Log.AppendText("writeFlash() for device " + index.ToString() + " result: " + result.ToString + vbCrLf)
                Else
                    Log.AppendText("writeFlash() for device " + index.ToString() + " failed. Error code: " + result.ToString + vbCrLf)
                End If

            End If
        Next
    End Sub

    Private Sub Form1_FormClosing(sender As Object, e As FormClosingEventArgs) Handles MyBase.FormClosing
        If _devicesCount > 0 Then
            For index As Byte = 0 To _devicesCount - 1
                spectrInterface.disconnectDeviceContext(_deviceContexts(index))
            Next
        End If
    End Sub
End Class

Public Class DeviceBuffers
    Public frameBuffer() As UShort
    Public flashBuffer() As Byte
End Class
