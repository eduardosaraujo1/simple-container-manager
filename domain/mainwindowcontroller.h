// Check out "Qt6 Dynamic Table Implementation" ChatGPT chat
// - Implement ContainerRowWidget
// - Implement ContainerListWidget
// - Put ContainerListWidget in mainWindow UI
// - Put DockerCLI, DockerEventStream and ApplicationPreferences in ContainerListWidget and use those as data sources
// - Write the function below
//function syncContainers()
// for each config entry at index i:
    // if widget exists:
        // update widget

        // if widget is not already at position i:
            // move it (insertWidget)
    // else:
        // create widget
        // insert at position i
// Ensure empty ContainerListWidget containers display a nice message
// Ensure errors are handled gracefully