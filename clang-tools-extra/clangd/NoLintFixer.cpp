//===--- NoLintFixes.h ------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "NoLintFixer.h"

#include "Diagnostics.h"
#include "FeatureModule.h"
#include "refactor/Tweak.h"
#include "support/Logger.h"

namespace clang {
namespace clangd {

namespace {
struct NoLintTweak : public Tweak {
  /// A unique id of the action, it is always equal to the name of the class
  /// defining the Tweak. Definition is provided automatically by
  /// REGISTER_TWEAK.
  const char *id() const override { return "NoLintTweak"; }
  /// Run the first stage of the action. Returns true indicating that the
  /// action is available and should be shown to the user. Returns false if
  /// the action is not available. This function should be fast, if the action
  /// requires non-trivial work it should be moved into 'apply'. Returns true
  /// iff the action is available and apply() can be called on it.
  bool prepare(const Selection &Sel) override {
    elog("NoLint: prepare {0}", Sel.Code);
    return true;
  }
  /// Run the second stage of the action that would produce the actual effect.
  /// EXPECTS: prepare() was called and returned true.
  Expected<Effect> apply(const Selection &Sel) override {
    elog("NoLint: apply {0}", Sel.Code);

    return Effect::showMessage("Placeholder Effect");
  }

  /// A one-line title of the action that should be shown to the users in the
  /// UI.
  /// EXPECTS: prepare() was called and returned true.
  std::string title() const override { return "NoLintTweakTitle"; }

  /// Describes what kind of action this is.
  /// EXPECTS: prepare() was called and returned true.
  llvm::StringLiteral kind() const override { return "NoLintTweakKind"; }
};

/// Extension point that allows modules to observe and modify an AST build.
/// One instance is created each time clangd produces a ParsedAST or
/// PrecompiledPreamble. For a given instance, lifecycle methods are always
/// called on a single thread.
struct Listener : public FeatureModule::ASTListener {
  /// Called before every AST build, both for main file and preamble. The call
  /// happens immediately before FrontendAction::Execute(), with Preprocessor
  /// set up already and after BeginSourceFile() on main file was called.
  void beforeExecute(CompilerInstance &CI) override {
    elog("NoLint: beforeExecute");
  }

  /// Called everytime a diagnostic is encountered. Modules can use this
  /// modify the final diagnostic, or store some information to surface code
  /// actions later on.
  void sawDiagnostic(const clang::Diagnostic &Info,
                     clangd::Diag &Diag) override {
    elog("NoLint: sawDiagnostic {0} {1}", Diag.Message, Diag.Source);
    if (Diag.Source == clangd::Diag::ClangTidy) {
      elog("NoLint: sawDiagnostic clang-tidy");
    }
  }
};
} // namespace

/// Called by the server to connect this feature module to LSP.
/// The module should register the methods/notifications/commands it handles,
/// and update the server capabilities to advertise them.
///
/// This is only called if the module is running in ClangdLSPServer!
/// FeatureModules with a public interface should work without LSP bindings.
void NoLintFixerModule::initializeLSP(LSPBinder &Bind,
                                      const llvm::json::Object &ClientCaps,
                                      llvm::json::Object &ServerCaps) {}

/// Tweaks implemented by this module. Can be called asynchronously when
/// enumerating or applying code actions.
void NoLintFixerModule::contributeTweaks(
    std::vector<std::unique_ptr<Tweak>> &Out) {
  Out.push_back(std::make_unique<NoLintTweak>());
}

/// Can be called asynchronously before building an AST.
std::unique_ptr<FeatureModule::ASTListener> NoLintFixerModule::astListeners() {
  return std::make_unique<Listener>();
}
} // namespace clangd
} // namespace clang
