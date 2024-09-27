#include "pch.h"
#include "LaunchingStatus.h"

#include <common/logger/logger.h>

LaunchingStatus::LaunchingStatus(const WorkspacesData::WorkspacesProject& project, std::function<void(const WorkspacesData::LaunchingAppStateMap&)> updateCallback) :
    m_updateCallback(updateCallback)
{
    std::unique_lock lock(m_mutex);
    for (const auto& app : project.apps)
    {
        m_appsState.insert({ app, { app, nullptr, LaunchingState::Waiting } });
    }
}

const WorkspacesData::LaunchingAppStateMap& LaunchingStatus::Get() noexcept
{
    std::shared_lock lock(m_mutex);
    return m_appsState;
}

bool LaunchingStatus::AllLaunchedAndMoved() noexcept
{
    std::shared_lock lock(m_mutex);
    for (const auto& [app, data] : m_appsState)
    {
        if (data.state != LaunchingState::Failed && data.state != LaunchingState::LaunchedAndMoved)
        {
            Logger::debug(data.state);
            return false;
        }
    }

    return true;
}

bool LaunchingStatus::AllLaunched() noexcept
{
    std::shared_lock lock(m_mutex);
    for (const auto& [app, data] : m_appsState)
    {
        if (data.state == LaunchingState::Waiting)
        {
            return false;
        }
    }

    return true;
}

void LaunchingStatus::UpdateLaunched(const WorkspacesData::WorkspacesProject::Application& app, LaunchingState state)
{
    std::unique_lock lock(m_mutex);
    Logger::error(L"UpdateLaunched :: trying to update state: app {} to {}", app.name, state);
    if (!m_appsState.contains(app))
    {
        Logger::error(L"Error updating state: app {} is not tracked in the project", app.name);
        return;
    }

    for (auto const& [key, val] : m_appsState)
    {
        if ((key == app) && (val.state == LaunchingState::Launched))
        {
            Logger::error(L"UpdateLaunched :: updating state: app {} to {}", app.name, state);
            m_appsState[key].state = state;
            if (m_updateCallback)
            {
                m_updateCallback(m_appsState);
            }
            return;
        }
    }
    Logger::error(L"UpdateLaunched :: error app not find {}", app.name);
}

void LaunchingStatus::Update(const WorkspacesData::WorkspacesProject::Application& app, LaunchingState state)
{
    std::unique_lock lock(m_mutex);
    Logger::error(L"updating state: app {} from {} to {}", app.name, m_appsState[app].state, state);
    if (!m_appsState.contains(app))
    {
        Logger::error(L"Error updating state: app {} is not tracked in the project", app.name);
        return;
    }

    m_appsState[app].state = state;

    if (m_updateCallback)
    {
        m_updateCallback(m_appsState);
    }
}

LaunchingState LaunchingStatus::GetStatus(const WorkspacesData::WorkspacesProject::Application& app)
{
    std::shared_lock lock(m_mutex);
    if (!m_appsState.contains(app))
    {
        Logger::error(L"Error getting state: app {} is not tracked in the project", app.name);
        return LaunchingState::Failed;
    }
    return m_appsState[app].state;
}

bool LaunchingStatus::ExistsSameAppLaunched(const WorkspacesData::WorkspacesProject::Application& appToFind)
{
    std::shared_lock lock(m_mutex);
    for (const auto& [app, data] : m_appsState)
    {
        if ((data.state == LaunchingState::Launched) && (app.path == appToFind.path))
        {
            return true;
        }
    }

    return false;
}
