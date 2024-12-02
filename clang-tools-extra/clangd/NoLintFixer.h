//===--- NoLintFixes.h ------------------------------------------*- C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANGD_NOLINTFIXER_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANGD_NOLINTFIXER_H

#include "Diagnostics.h"
#include "FeatureModule.h"
#include "refactor/Tweak.h"
namespace clang {
namespace clangd {
class NoLintFixerModule final : public FeatureModule {
public:
  /// Called by the server to connect this feature module to LSP.
  /// The module should register the methods/notifications/commands it handles,
  /// and update the server capabilities to advertise them.
  ///
  /// This is only called if the module is running in ClangdLSPServer!
  /// FeatureModules with a public interface should work without LSP bindings.
  void initializeLSP(LSPBinder &Bind, const llvm::json::Object &ClientCaps,
                     llvm::json::Object &ServerCaps) override;

  /// Tweaks implemented by this module. Can be called asynchronously when
  /// enumerating or applying code actions.
  void contributeTweaks(std::vector<std::unique_ptr<Tweak>> &Out) override;

  /// Can be called asynchronously before building an AST.
  std::unique_ptr<ASTListener> astListeners() override;
};
} // namespace clangd
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANGD_NOLINTFIXER_H
