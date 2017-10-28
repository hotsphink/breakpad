/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "client/linux/handler/exception_handler.h"

using google_breakpad::ExceptionHandler;

static ExceptionHandler* gExceptionHandler = nullptr;
static bool gEnabled = true;

bool TestEnabled(void* /* context */)
{
    return gEnabled;
}

extern "C" {
void EnableBreakpadExceptionHandler(bool enable);
}

void
EnableBreakpadExceptionHandler(bool enable)
{
    gEnabled = enable;
}

bool
SetGlobalExceptionHandler(ExceptionHandler::FilterCallback filterCallback,
                          ExceptionHandler::MinidumpCallback minidumpCallback
                          )
{
  const char *tempenv = getenv("TMPDIR");
  if (!tempenv)
    tempenv = "/tmp";
  char tempPath[PATH_MAX + 1];
  strcpy(tempPath, tempenv);
  strncat(tempPath, "/", sizeof(tempPath) - 1);

  google_breakpad::MinidumpDescriptor descriptor(tempPath);

  gExceptionHandler = new ExceptionHandler(
      descriptor,
      filterCallback,
      minidumpCallback,
      nullptr,
      true, -1
  );
  if (!gExceptionHandler)
    return false;

  return true;
}

// Called when loading the DLL (eg via LD_PRELOAD, or the JS shell --dll option).
void __attribute__((constructor))
SetBreakpadExceptionHandler()
{
    if (gExceptionHandler)
        abort();

    if (!SetGlobalExceptionHandler(nullptr, nullptr))
        abort();

   if (!gExceptionHandler)
       abort();
}
