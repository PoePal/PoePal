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
 
function Controller()
{
	if(installer.isInstaller())
	{
		installer.setDefaultPageVisible(QInstaller.ComponentSelection,0);
		installer.setDefaultPageVisible(QInstaller.TargetDirectory, 0);
		installer.setDefaultPageVisible(QInstaller.StartMenuSelection, 0);
	}
	if(installer.isUninstaller())
	{
        installer.setDefaultPageVisible(QInstaller.Introduction, false);
        installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
        installer.setDefaultPageVisible(QInstaller.LicenseCheck, false);
	}
}

Controller.prototype.IntroductionPageCallback = function()
{
	var path = installer.findPath("uninstall.exe", installer.value("TargetDir"));
	if(installer.fileExists(path) && !installer.isUninstaller())
	{
		QMessageBox.question("uninstallrequired", "Uninstall Required",
			"The previously installed version must be uninstalled first.", QMessageBox.Ok);
		installer.execute(path);
	}
	if(installer.value('isSilent'))
	{
		gui.clickButton(buttons.NextButton);
	}
}

Controller.prototype.LicenseAgreementPageCallback = function()
{
    var widget = gui.currentPageWidget();
    if (widget != null) {
        widget.AcceptLicenseRadioButton.checked = true;
    }
	if(installer.value('isSilent'))
	{
		gui.clickButton(buttons.NextButton);
	}
}

Controller.prototype.ReadyForInstallationPageCallback = function()
{
	if(installer.value('isSilent'))
	{
		gui.clickButton(buttons.NextButton);
	}
}
Controller.prototype.FinishedPageCallback = function()
{
	if(installer.value('isSilent'))
	{
		gui.clickButton(buttons.FinishButton);
	}
}
