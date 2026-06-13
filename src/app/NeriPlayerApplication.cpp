#include "app/NeriPlayerApplication.h"

#include "core/network/NetworkManager.h"
#include "mainwindow.h"

namespace NeriPlayerQt {

NeriPlayerApplication::NeriPlayerApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{
    setApplicationName(QStringLiteral("NeriPlayer Qt"));
    setApplicationVersion(QStringLiteral("0.1.0"));
    setOrganizationName(QStringLiteral("NeriPlayer"));
}

NeriPlayerApplication::~NeriPlayerApplication() = default;

bool NeriPlayerApplication::initialize()
{
    initializeCoreServices();
    initializeUi();
    return true;
}

void NeriPlayerApplication::showMainWindow()
{
    if (m_mainWindow) {
        m_mainWindow->show();
    }
}

ServiceLocator *NeriPlayerApplication::services()
{
    return &m_services;
}

const ServiceLocator *NeriPlayerApplication::services() const
{
    return &m_services;
}

MainWindow *NeriPlayerApplication::mainWindow() const
{
    return m_mainWindow.get();
}

void NeriPlayerApplication::initializeCoreServices()
{
    m_services.registerService<NetworkManager>(std::make_unique<NetworkManager>());
}

void NeriPlayerApplication::initializeUi()
{
    m_mainWindow = std::make_unique<MainWindow>();
}

} // namespace NeriPlayerQt
