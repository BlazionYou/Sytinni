﻿using System;
using UtinniCore.Utinni;
using UtinniCoreDotNet.Callbacks;
using UtinniCoreDotNet.Commands;
using UtinniCoreDotNet.PluginFramework;
using UtinniCoreDotNet.Utility;

namespace ExampleEditorPlugin
{
    // For an Editor, either inherit EditorPluginBase or IEditorPlugin.
    // As all plugins with the IEditorPlugin interface get automatically
    // added to the main Editor windows flowlayoutpanel as an CollapsiblePanel,
    // it's preferred to inherit EditorPluginBase, as it sets up the the correct size
    // automatically.
    public partial class ExampleEditorPlugin : EditorPluginBase
    {
        public ExampleEditorPlugin()
        {
            InitializeComponent();

            // The only thing that needs to be set up is the PluginInformation and the plugin is good to go
            Information = new PluginInformation("Example Editor Plugin"
                                                , "This is an editor plugin example"
                                                , "Example Author");


            Log.Info("Example Editor Plugin created");
        }

        private void btnAddWsNode_Click(object sender, EventArgs e)
        {
            GroundSceneCallbacks.AddUpdateLoopCall(() =>
            {
                WorldSnapshotReaderWriter.Node node = WorldSnapshot.CreateAddNode("object/tangible/furniture/elegant/shared_chair_s01.iff", Game.Player.ObjectToParent);

                if (node != null)
                {
                    AddUndoCommand(this, new AddUndoCommandEventArgs(new AddWorldSnapshotNodeCommand(node)));
                }
            });
        }

        private void btnRemoveWsNode_Click(object sender, EventArgs e)
        {
            GroundSceneCallbacks.AddUpdateLoopCall(() =>
            {
                WorldSnapshotReaderWriter.Node node = WorldSnapshotReaderWriter.Get().GetNodeByNetworkId(Game.PlayerLookAtTargetObject.NetworkId);

                if (node != null)
                {
                    AddUndoCommand(this, new AddUndoCommandEventArgs(new RemoveWorldSnapshotNodeCommand(node)));
                }

                WorldSnapshot.RemoveNode(node);
            });
        }
    }
}
