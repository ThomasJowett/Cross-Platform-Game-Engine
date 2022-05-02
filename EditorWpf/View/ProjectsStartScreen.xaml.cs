using System.Windows;

namespace Editor.View
{
    /// <summary>
    /// Interaction logic for ProjectsStartScreen.xaml
    /// </summary>
    public partial class ProjectsStartScreen : Window
    {
        public ProjectsStartScreen()
        {
            InitializeComponent();
        }

        private void openProjectButton_Click(object sender, RoutedEventArgs e)
        {
            if (createProjectButton.IsChecked == true)
            {
                createProjectButton.IsChecked = false;
                browserPanel.Margin = new Thickness(0, 0, 0, 0);
            }
            openProjectButton.IsChecked = true;
        }

        private void createProjectButton_Click(object sender, RoutedEventArgs e)
        {
            if (openProjectButton.IsChecked == true)
            {
                openProjectButton.IsChecked = false;
                browserPanel.Margin = new Thickness(-800, 0, 0, 0);
            }
            createProjectButton.IsChecked = true;
        }
    }
}
