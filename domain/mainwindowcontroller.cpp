#include "mainwindowvmodel.h"

MainWindowVModel::MainWindowVModel(
    QObject *parent,
    AppPreferences *preferences,
    DockerCLI *dockercli,
    DockerEventStream *dockerEventStream
    )  : m_preferences(std::move(preferences))
       , m_dockercli(std::move(dockercli))
       , m_dockerEventStream(std::move(dockerEventStream))
       , QObject{parent}
{}
