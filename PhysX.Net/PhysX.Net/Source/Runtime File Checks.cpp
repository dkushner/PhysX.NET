#include "StdAfx.h"
#include "Runtime File Checks.h"

using namespace System;
using namespace System::Globalization;
using namespace System::Diagnostics;
using namespace System::IO;
using namespace System::Reflection;
using namespace System::Security::Cryptography;
using namespace StillDesign::PhysX;

void RuntimeFileChecks::Check()
{
#if _WIN64
	CheckFile( "PhysXCore64.dll", PhysXDllVersion );
	CheckFile( "PhysXCooking64.dll", PhysXDllVersion );
	CheckFile( "cudart64_30_9.dll", CudaDllVersion );
	CheckFile( "PhysXDevice64.dll", PhysXDeviceDllVersion );
	CheckFile( "PhysXLoader64.dll", PhysXDllVersion );
	CheckFile( "NxCharacter64.dll", PhysXDllVersion );
#elif WIN32
	CheckFile( "PhysXCore.dll", PhysXDllVersion );
	CheckFile( "PhysXCooking.dll", PhysXDllVersion );
	CheckFile( "cudart32_30_9.dll", CudaDllVersion );
	CheckFile( "PhysXDevice.dll", PhysXDeviceDllVersion );
	CheckFile( "PhysXLoader.dll", PhysXDllVersion );
#endif
}

void RuntimeFileChecks::CheckFile( String^ filename, String^ requiredVersion )
{
	CheckFile( filename, gcnew Version( requiredVersion ) );
}
void RuntimeFileChecks::CheckFile( String^ filename, Version^ requiredVersion )
{
	try
	{
		String^ filePath = FindLibraryPath( filename );
		Version^ version = GetFileVersion( filePath );
		
		// Compare the file version of the DLL to the required version
		if( version != requiredVersion )
			throw gcnew DllNotFoundException( String::Format( "PhysX library \"{0}\" is not the correct version. Given: {1}. Required: {2}.", filename, version, requiredVersion ) );
	}
	catch( FileNotFoundException^ exception )
	{
		throw gcnew DllNotFoundException( String::Format( "PhysX library \"{0}\" is missing.", filename ), exception );
	}
	catch( Exception^ ex )
	{
		throw ex;
	}
	finally
	{
		
	}
}

String^ RuntimeFileChecks::FindLibraryPath( System::String^ name )
{
	Assembly^ assembly = GetBestAssembly();
	
	String^ path;
	
	Uri^ executableUri = gcnew Uri( assembly->CodeBase );
	path = Path::Combine( Path::GetDirectoryName( executableUri->LocalPath ), name );
	if ( File::Exists( path ) )
		return path;

	path = Path::Combine( Environment::SystemDirectory, name );
	if ( File::Exists( path ) )
		return path;

	path = Path::Combine( GetEnvironmentPathVariable( "WINDIR" ), name );
	if ( File::Exists( path ) )
		return path;

	path = Path::Combine( Environment::CurrentDirectory, name );
	if ( File::Exists( path ) )
		return path;
	
	array<String^>^ environmentPaths = GetEnvironmentPathVariable( "PATH" )->Split( gcnew array<Char>( 1 ) { ';' }, StringSplitOptions::RemoveEmptyEntries );
	for each ( String^ envPath in environmentPaths )
	{
		path = Path::Combine( envPath, name );
		if ( File::Exists( path ) )
			return path;
	}

	throw gcnew FileNotFoundException();
}

Assembly^ RuntimeFileChecks::GetBestAssembly()
{
	if( Assembly::GetEntryAssembly() != nullptr )
		return Assembly::GetEntryAssembly();
	
	if( Assembly::GetExecutingAssembly() != nullptr )
		return Assembly::GetExecutingAssembly();
	
	throw gcnew PhysXException( "Could not find suitable assembly" );
}

#pragma push_macro( "GetEnvironmentVariable" )
#undef GetEnvironmentVariable

String^ RuntimeFileChecks::GetEnvironmentPathVariable( String^ name )
{
	if( String::IsNullOrEmpty( name ) )
		throw gcnew ArgumentNullException( "name" );
	
	String^ variable = Environment::GetEnvironmentVariable( name );
	
	variable = variable->Replace( "\"", String::Empty );
	variable = variable->Replace( "'", String::Empty );
	variable = variable->Trim();
	
	return variable;
}
#pragma pop_macro( "GetEnvironmentVariable" )

Version^ RuntimeFileChecks::GetFileVersion( String^ filename )
{
	FileVersionInfo^ versionInfo = FileVersionInfo::GetVersionInfo( filename );
	
	return gcnew Version( versionInfo->FileMajorPart, versionInfo->FileMinorPart, versionInfo->FileBuildPart, versionInfo->FilePrivatePart );
}