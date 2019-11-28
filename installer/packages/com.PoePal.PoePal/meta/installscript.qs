/**
 * PoePal - A companion application to Path of Exile.
 * Copyright (C) 2019 Phillip Doup (douppc@gmail.com)
 *
 * PoePal is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) 
 * any later version.
 *
 * PoePal is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the 
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along with PeoPal.  If not, see 
 * <https://www.gnu.org/licenses/>.
 */

function Component()
{
    installer.installationFinished.connect(this, Component.prototype.installationFinishedPageIsShown);
    installer.finishButtonClicked.connect(this, Component.prototype.installationFinished);
}

Component.prototype.isDefault = function()
{
    // select the component by default
    return true;
}

Component.prototype.installationFinishedPageIsShown = function()
{
    try {
        if (installer.isInstaller() && installer.status == QInstaller.Success) {
            installer.addWizardPageItem( component, "StartAfterForm", QInstaller.InstallationFinished );
        }
    } catch(e) {
        console.log(e);
    }
}

Component.prototype.createOperations = function()
{
    try {
        // call the base create operations function
        component.createOperations();
		
        component.addOperation("CreateShortcut", "@TargetDir@/bin/PoePal.exe", "@UserStartMenuProgramsPath@/PoePal.lnk",
            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/bin/PoePal.exe",
            "iconId=0", "description=Run PoePal");
		
		
    } catch (e) {
        console.log(e);
    }
}

Component.prototype.installationFinished = function()
{
    try {
        if (installer.isInstaller() && installer.status == QInstaller.Success) {
            var isReadMeCheckBoxChecked = component.userInterface( "StartAfterForm" ).startAfterCheck.checked;
            if (isReadMeCheckBoxChecked) 
			{
				installer.executeDetached(installer.value("TargetDir")+"/bin/PoePal.exe");
            }
        }
    } catch(e) {
        console.log(e);
    }
}
