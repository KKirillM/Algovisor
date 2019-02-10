set objArgs = WScript.Arguments
set o_installer = CreateObject("WindowsInstaller.Installer")
set database = o_Installer.OpenDatabase(objArgs(0), 1)
query = "INSERT INTO `LockPermissions` (`LockObject`,`Table`,`User`,`Permission`) VALUES ('TARGETDIR','CreateFolder','Everyone',268435456)"
Set view = database.OpenView (query)         
view.Execute
database.Commit