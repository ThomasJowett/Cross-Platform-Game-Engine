using Editor.View;
using System.Windows;

namespace Editor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            Loaded += OnMainWindowLoaded;

            int test = DLLWrapper.EngineAPI.GetValueFromDLL();
        }

        private void OnMainWindowLoaded(object sender, RoutedEventArgs e)
        {
            Loaded -= OnMainWindowLoaded;
            OpenProjectStartScreen();
        }

        private void OpenProjectStartScreen()
        {
            var projectStartScreen = new ProjectsStartScreen();
            if(projectStartScreen.ShowDialog() == false)
            {
                Application.Current.Shutdown();
            }
            else
            {

            }
        }
    }
}
