<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Me.ConnectButton = New System.Windows.Forms.Button()
        Me.StatusButton = New System.Windows.Forms.Button()
        Me.TriggerButton = New System.Windows.Forms.Button()
        Me.GetFrameButton = New System.Windows.Forms.Button()
        Me.ClearMemoryButton = New System.Windows.Forms.Button()
        Me.GetParametersButton = New System.Windows.Forms.Button()
        Me.SetParametersButton = New System.Windows.Forms.Button()
        Me.EraseFlashButton = New System.Windows.Forms.Button()
        Me.ReadFlashButton = New System.Windows.Forms.Button()
        Me.WriteFlashButton = New System.Windows.Forms.Button()
        Me.Log = New System.Windows.Forms.RichTextBox()
        Me.SuspendLayout()
        '
        'ConnectButton
        '
        Me.ConnectButton.Location = New System.Drawing.Point(296, 21)
        Me.ConnectButton.Name = "ConnectButton"
        Me.ConnectButton.Size = New System.Drawing.Size(81, 23)
        Me.ConnectButton.TabIndex = 0
        Me.ConnectButton.Text = "Connect"
        Me.ConnectButton.UseVisualStyleBackColor = True
        '
        'StatusButton
        '
        Me.StatusButton.Location = New System.Drawing.Point(69, 354)
        Me.StatusButton.Name = "StatusButton"
        Me.StatusButton.Size = New System.Drawing.Size(93, 23)
        Me.StatusButton.TabIndex = 2
        Me.StatusButton.Text = "Get status"
        Me.StatusButton.UseVisualStyleBackColor = True
        '
        'TriggerButton
        '
        Me.TriggerButton.Location = New System.Drawing.Point(199, 383)
        Me.TriggerButton.Name = "TriggerButton"
        Me.TriggerButton.Size = New System.Drawing.Size(69, 23)
        Me.TriggerButton.TabIndex = 4
        Me.TriggerButton.Text = "Trigger"
        Me.TriggerButton.UseVisualStyleBackColor = True
        '
        'GetFrameButton
        '
        Me.GetFrameButton.Location = New System.Drawing.Point(302, 383)
        Me.GetFrameButton.Name = "GetFrameButton"
        Me.GetFrameButton.Size = New System.Drawing.Size(75, 23)
        Me.GetFrameButton.TabIndex = 5
        Me.GetFrameButton.Text = "Get Frame"
        Me.GetFrameButton.UseVisualStyleBackColor = True
        '
        'ClearMemoryButton
        '
        Me.ClearMemoryButton.Location = New System.Drawing.Point(510, 383)
        Me.ClearMemoryButton.Name = "ClearMemoryButton"
        Me.ClearMemoryButton.Size = New System.Drawing.Size(89, 23)
        Me.ClearMemoryButton.TabIndex = 6
        Me.ClearMemoryButton.Text = "Clear memory"
        Me.ClearMemoryButton.UseVisualStyleBackColor = True
        '
        'GetParametersButton
        '
        Me.GetParametersButton.Location = New System.Drawing.Point(69, 383)
        Me.GetParametersButton.Name = "GetParametersButton"
        Me.GetParametersButton.Size = New System.Drawing.Size(93, 23)
        Me.GetParametersButton.TabIndex = 7
        Me.GetParametersButton.Text = "Get parameters"
        Me.GetParametersButton.UseVisualStyleBackColor = True
        '
        'SetParametersButton
        '
        Me.SetParametersButton.Location = New System.Drawing.Point(69, 412)
        Me.SetParametersButton.Name = "SetParametersButton"
        Me.SetParametersButton.Size = New System.Drawing.Size(93, 23)
        Me.SetParametersButton.TabIndex = 8
        Me.SetParametersButton.Text = "Set parameters"
        Me.SetParametersButton.UseVisualStyleBackColor = True
        '
        'EraseFlashButton
        '
        Me.EraseFlashButton.Location = New System.Drawing.Point(403, 354)
        Me.EraseFlashButton.Name = "EraseFlashButton"
        Me.EraseFlashButton.Size = New System.Drawing.Size(75, 23)
        Me.EraseFlashButton.TabIndex = 9
        Me.EraseFlashButton.Text = "Erase Flash"
        Me.EraseFlashButton.UseVisualStyleBackColor = True
        '
        'ReadFlashButton
        '
        Me.ReadFlashButton.Location = New System.Drawing.Point(403, 383)
        Me.ReadFlashButton.Name = "ReadFlashButton"
        Me.ReadFlashButton.Size = New System.Drawing.Size(75, 23)
        Me.ReadFlashButton.TabIndex = 10
        Me.ReadFlashButton.Text = "Read Flash"
        Me.ReadFlashButton.UseVisualStyleBackColor = True
        '
        'WriteFlashButton
        '
        Me.WriteFlashButton.Location = New System.Drawing.Point(403, 412)
        Me.WriteFlashButton.Name = "WriteFlashButton"
        Me.WriteFlashButton.Size = New System.Drawing.Size(75, 23)
        Me.WriteFlashButton.TabIndex = 11
        Me.WriteFlashButton.Text = "Write Flash"
        Me.WriteFlashButton.UseVisualStyleBackColor = True
        '
        'Log
        '
        Me.Log.Location = New System.Drawing.Point(38, 69)
        Me.Log.Name = "Log"
        Me.Log.Size = New System.Drawing.Size(628, 266)
        Me.Log.TabIndex = 12
        Me.Log.Text = ""
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(699, 493)
        Me.Controls.Add(Me.Log)
        Me.Controls.Add(Me.WriteFlashButton)
        Me.Controls.Add(Me.ReadFlashButton)
        Me.Controls.Add(Me.EraseFlashButton)
        Me.Controls.Add(Me.SetParametersButton)
        Me.Controls.Add(Me.GetParametersButton)
        Me.Controls.Add(Me.ClearMemoryButton)
        Me.Controls.Add(Me.GetFrameButton)
        Me.Controls.Add(Me.TriggerButton)
        Me.Controls.Add(Me.StatusButton)
        Me.Controls.Add(Me.ConnectButton)
        Me.Name = "Form1"
        Me.Text = "Form1"
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents ConnectButton As System.Windows.Forms.Button
    Friend WithEvents StatusButton As System.Windows.Forms.Button
    Friend WithEvents TriggerButton As System.Windows.Forms.Button
    Friend WithEvents GetFrameButton As System.Windows.Forms.Button
    Friend WithEvents ClearMemoryButton As System.Windows.Forms.Button
    Friend WithEvents GetParametersButton As System.Windows.Forms.Button
    Friend WithEvents SetParametersButton As System.Windows.Forms.Button
    Friend WithEvents EraseFlashButton As System.Windows.Forms.Button
    Friend WithEvents ReadFlashButton As System.Windows.Forms.Button
    Friend WithEvents WriteFlashButton As System.Windows.Forms.Button
    Friend WithEvents Log As System.Windows.Forms.RichTextBox
End Class
