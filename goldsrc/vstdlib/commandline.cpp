/*
 * This file is part of OGS Engine
 * Copyright (C) 2024 BlackPhrase
 *
 * OGS Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OGS Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OGS Engine. If not, see <http://www.gnu.org/licenses/>.
 */

/// @file

#include <vstdlib/icommandline.h>

CCommandLineParam::CCommandLineParam(const char *asSomething, const char *asSomething)
{
	// TODO
};

CCommandLineParam::~CCommandLineParam()
{
	// TODO
};

bool CCommandLineParam::Exists()
{
	// TODO
	return false;
};

int CCommandLineParam::GetHParam()
{
	// TODO
	return 0;
};

class CCommandLine : public ICommandLine
{
public:
	void CreateCmdLine(const char *asLine) override;
	void CreateCmdLine(int argc, char **argv) override;
	
	const char *GetCmdLine() const override;
	
	const char *CheckParm(const char *asParm, const char **apValue) const override;
	void RemoveParm(const char *asParm) override;
	void AppendParm(const char *asParm, const char *asValues) override;
	
	const char *ParmValue(const char *asParm, const char *asDefaultValue) const override;
	int ParmValue(const char *asParm, int anDefaultValue) const override;
	float ParmValue(const char *asParm, float afDefaultValue) const override;
	
	int ParmCount() const override;
	
	int FindParm(const char *asParm) const override;
	const char *GetParm(int anIndex) const override;
};

void CCommandLine::CreateCmdLine(const char *asLine)
{
	// TODO
};

void CCommandLine::CreateCmdLine(int argc, char **argv)
{
	// TODO
};

const char *CCommandLine::GetCmdLine() const
{
	// TODO
	return "";
};

const char *CCommandLine::CheckParm(const char *asParm, const char **apValue) const
{
	// TODO
	return "";
};

void CCommandLine::RemoveParm(const char *asParm)
{
	// TODO
};

void CCommandLine::AppendParm(const char *asParm, const char *asValues)
{
	// TODO
};

const char *CCommandLine::ParmValue(const char *asParm, const char *asDefaultValue) const
{
	// TODO
	return "";
};

int CCommandLine::ParmValue(const char *asParm, int anDefaultValue) const
{
	// TODO
	return 0;
};

float CCommandLine::ParmValue(const char *asParm, float afDefaultValue) const
{
	// TODO
	return 0.0f;
};

int CCommandLine::ParmCount() const
{
	// TODO
	return 0;
};

int CCommandLine::FindParm(const char *asParm) const
{
	// TODO
	return -1;
};

const char *CCommandLine::GetParm(int anIndex) const
{
	// TODO
	return "";
};

CCommandLine gCommandLine;

ICommandLine *CommandLine()
{
	return &gCommandLine;
};