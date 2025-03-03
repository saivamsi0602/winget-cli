// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "ConfigureCommand.h"
#include "ConfigureShowCommand.h"
#include "ConfigureTestCommand.h"
#include "ConfigureValidateCommand.h"
#include "Workflows/ConfigurationFlow.h"
#include "Workflows/MSStoreInstallerHandler.h"

using namespace AppInstaller::CLI::Workflow;

namespace AppInstaller::CLI
{
    ConfigureCommand::ConfigureCommand(std::string_view parent) :
        Command("configure", { "configuration" }, parent, Settings::ExperimentalFeature::Feature::Configuration)
    {
        SelectCurrentCommandIfUnrecognizedSubcommandFound(true);
    }

    std::vector<std::unique_ptr<Command>> ConfigureCommand::GetCommands() const
    {
        return InitializeFromMoveOnly<std::vector<std::unique_ptr<Command>>>({
            std::make_unique<ConfigureShowCommand>(FullName()),
            std::make_unique<ConfigureTestCommand>(FullName()),
            std::make_unique<ConfigureValidateCommand>(FullName()),
        });
    }

    std::vector<Argument> ConfigureCommand::GetArguments() const
    {
        return {
            Argument{ Execution::Args::Type::ConfigurationFile, Resource::String::ConfigurationFileArgumentDescription, ArgumentType::Positional },
            Argument{ Execution::Args::Type::ConfigurationAcceptWarning, Resource::String::ConfigurationAcceptWarningArgumentDescription, ArgumentType::Flag },
            Argument{ Execution::Args::Type::ConfigurationEnable, Resource::String::ConfigurationEnableMessage, ArgumentType::Flag, Argument::Visibility::Help },
            Argument{ Execution::Args::Type::ConfigurationDisable, Resource::String::ConfigurationDisableMessage, ArgumentType::Flag, Argument::Visibility::Help },
        };
    }

    Resource::LocString ConfigureCommand::ShortDescription() const
    {
        return { Resource::String::ConfigureCommandShortDescription };
    }

    Resource::LocString ConfigureCommand::LongDescription() const
    {
        return { Resource::String::ConfigureCommandLongDescription };
    }

    Utility::LocIndView ConfigureCommand::HelpLink() const
    {
        // TODO: Make this exist
        return "https://aka.ms/winget-command-configure"_liv;
    }

    void ConfigureCommand::ExecuteInternal(Execution::Context& context) const
    {
        if (context.Args.Contains(Execution::Args::Type::ConfigurationEnable))
        {
            context <<
                EnableConfiguration;
        }
        else if (context.Args.Contains(Execution::Args::Type::ConfigurationDisable))
        {
            context <<
                DisableConfiguration;
        }
        else
        {
            context <<
                VerifyIsFullPackage <<
                VerifyFile(Execution::Args::Type::ConfigurationFile) <<
                CreateConfigurationProcessor <<
                OpenConfigurationSet <<
                ShowConfigurationSet <<
                ShowConfigurationSetConflicts <<
                ConfirmConfigurationProcessing <<
                ApplyConfigurationSet;
        }
    }

    void ConfigureCommand::ValidateArgumentsInternal(Execution::Args& execArgs) const
    {
        if (execArgs.Contains(Execution::Args::Type::ConfigurationEnable) ||
            execArgs.Contains(Execution::Args::Type::ConfigurationDisable))
        {
            if (execArgs.GetArgsCount() > 1)
            {
                throw CommandException(Resource::String::ConfigurationEnableArgumentError);
            }
        }
        else if (!execArgs.Contains(Execution::Args::Type::ConfigurationFile))
        {
            throw CommandException(Resource::String::RequiredArgError("file"_liv));
        }
    }
}
