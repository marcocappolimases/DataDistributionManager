# DataDistributionManager example 

## .NET

To execute the *dotnet* example do the following steps:
1. open a windows command prompt (cmd1)
2. cd to the examples/dotnet/TransmitterReceiver folder

```
$> cd [PATH TO EXAMPLES FOLDER]
$> cd dotnet\TransmitterReceiver
```

3. build the .NET solution
```
$> dotnet build TransmitterReceiver.csproj
```

This command will restore the **NuGet DataDistributionManager package** and build project for all the frameworks declared in the project, a **TransmitterReceiver.exe** file will be placed in the framework specific output folder. (e.g. .\dotnet\TransmitterReceiver\bin\Debug\net5.0\)

4. cd to the output folder
```
$> cd bin\Debug\net5.0\
```
5. open another windows command prompt and **cd to the output folder**
6. in the first command prompt execute the example as listener
```
$> TransmitterReceiver.exe
```
This will also start the DCPSInfoRepo.exe

6. in the second command prompt execute the example as transmitter
```
$> TransmitterReceiver.exe t
```
This start the application in transmission mode, in the other command prompt is visible the start of communications.
