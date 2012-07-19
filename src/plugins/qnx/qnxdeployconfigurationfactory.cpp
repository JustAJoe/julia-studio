/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (C) 2011 - 2012 Research In Motion
**
** Contact: Research In Motion (blackberry-qt@qnx.com)
** Contact: KDAB (info@kdab.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at info@qt.nokia.com.
**
**************************************************************************/

#include "qnxdeployconfigurationfactory.h"

#include "qnxconstants.h"
#include "qnxdeployconfiguration.h"
#include "qnxdeviceconfigurationfactory.h"

#include <projectexplorer/profileinformation.h>
#include <projectexplorer/target.h>
#include <remotelinux/genericdirectuploadstep.h>

using namespace Qnx;
using namespace Qnx::Internal;

QnxDeployConfigurationFactory::QnxDeployConfigurationFactory(QObject *parent)
    : ProjectExplorer::DeployConfigurationFactory(parent)
{
}

QList<Core::Id> QnxDeployConfigurationFactory::availableCreationIds(ProjectExplorer::Target *parent) const
{
    QList<Core::Id> ids;

    if (canHandle(parent))
        ids << Core::Id(Constants::QNX_QNX_DEPLOYCONFIGURATION_ID);

    return ids;
}

QString QnxDeployConfigurationFactory::displayNameForId(const Core::Id id) const
{
    if (id.toString().startsWith(QLatin1String(Constants::QNX_QNX_DEPLOYCONFIGURATION_ID)))
        return tr("Deploy to QNX Device");

    return QString();
}

bool QnxDeployConfigurationFactory::canCreate(ProjectExplorer::Target *parent, const Core::Id id) const
{
    if (!canHandle(parent)
            || !id.toString().startsWith(QLatin1String(Constants::QNX_QNX_DEPLOYCONFIGURATION_ID)))
        return false;

    return true;
}

ProjectExplorer::DeployConfiguration *QnxDeployConfigurationFactory::create(ProjectExplorer::Target *parent, const Core::Id id)
{
    if (!canCreate(parent, id))
        return 0;

    ProjectExplorer::DeployConfiguration * const dc = new QnxDeployConfiguration(parent, id,
        displayNameForId(id));
    dc->stepList()->insertStep(0, new RemoteLinux::GenericDirectUploadStep(dc->stepList(),
        RemoteLinux::GenericDirectUploadStep::stepId()));
    return dc;
}

bool QnxDeployConfigurationFactory::canRestore(ProjectExplorer::Target *parent, const QVariantMap &map) const
{
    return canCreate(parent, ProjectExplorer::idFromMap(map));
}

ProjectExplorer::DeployConfiguration *QnxDeployConfigurationFactory::restore(ProjectExplorer::Target *parent, const QVariantMap &map)
{
    if (!canRestore(parent, map))
        return 0;
    Core::Id id = ProjectExplorer::idFromMap(map);
    QnxDeployConfiguration * const dc = new QnxDeployConfiguration(parent, id, displayNameForId(id));
    if (!dc->fromMap(map)) {
        delete dc;
        return 0;
    }
    return dc;
}

bool QnxDeployConfigurationFactory::canClone(ProjectExplorer::Target *parent, ProjectExplorer::DeployConfiguration *source) const
{
    return canCreate(parent, source->id());
}

ProjectExplorer::DeployConfiguration *QnxDeployConfigurationFactory::clone(ProjectExplorer::Target *parent, ProjectExplorer::DeployConfiguration *source)
{
    if (!canClone(parent, source))
        return 0;
    return new QnxDeployConfiguration(parent, qobject_cast<QnxDeployConfiguration *>(source));
}

bool QnxDeployConfigurationFactory::canHandle(ProjectExplorer::Target *t) const
{
    Core::Id deviceType = ProjectExplorer::DeviceTypeProfileInformation::deviceTypeId(t->profile());
    if (deviceType != QnxDeviceConfigurationFactory::deviceType())
        return false;

    return true;
}