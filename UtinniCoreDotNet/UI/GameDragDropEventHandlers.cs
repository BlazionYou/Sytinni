﻿using System;
using System.Windows.Forms;
using UtinniCoreDotNet.UI.Controls;

namespace UtinniCoreDotNet.UI
{
    public static class GameDragDropEventHandlers
    {
        public static DragEventHandler OnDragDrop;
        public static DragEventHandler OnDragEnter;
        public static EventHandler OnDragLeave;
        public static DragEventHandler OnDragOver;

        public static void Initialize(PanelGame panelGame)
        {
            panelGame.DragDrop += OnDragDrop;
            panelGame.DragEnter += OnDragEnter;
            panelGame.DragLeave += OnDragLeave;
            panelGame.DragOver += OnDragOver;
        }
    }
}
