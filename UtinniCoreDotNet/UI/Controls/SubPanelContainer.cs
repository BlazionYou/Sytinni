﻿using System.Windows.Forms;
using UtinniCoreDotNet.UI.Theme;

namespace UtinniCoreDotNet.UI.Controls
{
    public class SubPanelContainer : FlowLayoutPanel
    {
        public SubPanelContainer(string text)
        {
            InitializeComponent();
            base.BackColor = Colors.Primary();
            base.ForeColor = Colors.Font();
            base.Text = text;
        }

        public SubPanelContainer(string text, SubPanel[] subPanels)
        {
            InitializeComponent();
            base.Text = text;
            SuspendLayout();
            foreach (SubPanel subPanel in subPanels)
            {
                Controls.Add(new CollapsiblePanel(subPanel, subPanel.CheckboxPanelText));
            }
            ResumeLayout();
        }

        private void InitializeComponent()
        {
            Location = new System.Drawing.Point(0, 30);
            AutoSize = true;
            AutoSizeMode = AutoSizeMode.GrowAndShrink;
            MinimumSize = new System.Drawing.Size(420, 0);
            Size = new System.Drawing.Size(420, 0);
            FlowDirection = FlowDirection.TopDown;
            WrapContents = false;
        }
    }
}
