// Copyright (c) Microsoft Corporation
// The Microsoft Corporation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

using System.Drawing;
using System.Windows;

namespace WorkspacesEditor
{
    /// <summary>
    /// Interaction logic for OverlayWindow.xaml
    /// </summary>
    public partial class OverlayWindow : Window
    {
        private Rectangle _bounds;

        public OverlayWindow(Rectangle bounds)
        {
            _bounds = bounds;
            InitializeComponent();
            Loaded += OverlayWindow_Loaded;
        }

        private void OverlayWindow_Loaded(object sender, RoutedEventArgs e)
        {
            Top = _bounds.Top;
            Left = _bounds.Left;
            Height = _bounds.Height;
            Width = _bounds.Width;
        }
    }
}
