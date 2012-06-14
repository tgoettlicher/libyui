/*
  |****************************************************************************
  |
  | Copyright (c) 2000 - 2012 Novell, Inc.
  | All Rights Reserved.
  |
  | This program is free software; you can redistribute it and/or
  | modify it under the terms of version 2 of the GNU General Public License as
  | published by the Free Software Foundation.
  |
  | This program is distributed in the hope that it will be useful,
  | but WITHOUT ANY WARRANTY; without even the implied warranty of
  | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
  | GNU General Public License for more details.
  |
  | You should have received a copy of the GNU General Public License
  | along with this program; if not, contact Novell, Inc.
  |
  | To contact Novell about this file by physical or electronic mail,
  | you may find current contact information at www.novell.com
  |
  |****************************************************************************
*/


/*-/

  File:		YUIPlugin.h

  Author:	Stefan Hundhammer <sh@suse.de>

/-*/


#include <dlfcn.h>

#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUIPlugin.h"
#include "YPath.h"

#include "Libyui_config.h"


YUIPlugin::YUIPlugin( const char * pluginLibBaseName )
{
    _pluginLibBaseName = std::string( pluginLibBaseName );

    std::string pluginFilename = pluginLibFullPath();

    _pluginLibHandle = dlopen( pluginFilename.c_str(),
			       RTLD_NOW | RTLD_GLOBAL);

    if ( ! _pluginLibHandle )
    {
	_errorMsg = dlerror();

	yuiError() << "Could not load UI plugin \"" << pluginLibBaseName
		   << "\": " << _errorMsg
		   << std::endl;
    }
}


YUIPlugin::~YUIPlugin()
{
    // This intentionally does NOT call unload(): This would be
    // counterproductive for almost all use cases of this class.
}


void
YUIPlugin::unload()
{
    if ( _pluginLibHandle )
	dlclose( _pluginLibHandle );
}


std::string
YUIPlugin::pluginLibFullPath() const
{

    std::string pluginName = PLUGIN_PREFIX;
    pluginName.append( _pluginLibBaseName );
    pluginName.append( PLUGIN_SUFFIX );

    YPath plugin( PLUGINDIR, pluginName );

    return plugin.path();
}


void * YUIPlugin::locateSymbol( const char * symbol )
{
    if ( ! _pluginLibHandle )
	return 0;

    void * addr = dlsym( _pluginLibHandle, symbol );

    if ( ! addr )
    {
	yuiError() << "Could not locate symbol \"" << symbol
		   << "\" in " << pluginLibFullPath()
		   << std::endl;
    }

    return addr;
}


bool YUIPlugin::error() const
{
    return _pluginLibHandle == 0;
}


bool YUIPlugin::success() const
{
    return _pluginLibHandle != 0;
}


std::string YUIPlugin::errorMsg() const
{
    return _errorMsg;
}
