#include "FileDialog.h"
#include <AppKit/AppKit.h>
#include <Foundation/Foundation.h>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <objc/objc.h>
#include <optional>

#if defined(__APPLE__)

#import <Cocoa/Cocoa.h>
#include <string>
#include <vector>

static NSString *WCharToNSString(const wchar_t *wstr) {
  if (!wstr)
    return nil;

  // Calculate length of wide string
  size_t len = wcslen(wstr);
  if (len == 0)
    return @"";

  // Directly initialize using UTF-32 Little Endian (Native for Apple Silicon &
  // Intel)
  return [[NSString alloc] initWithBytes:wstr
                                  length:len * sizeof(wchar_t)
                                encoding:NSUTF32LittleEndianStringEncoding];
}

static std::wstring NSStringToWString(NSString *str) {
  if (!str)
    return L"";

  // Ask NSString for raw UTF-32 bytes
  NSData *data = [str dataUsingEncoding:NSUTF32LittleEndianStringEncoding];
  if (!data)
    return L"";

  size_t len = [data length] / sizeof(wchar_t);
  std::wstring result(len, L'\0');

  [data getBytes:&result[0] length:[data length]];

  return result;
}

static NSMutableArray<NSString *> *ParseFilter(const wchar_t *filter) {
  NSMutableArray<NSString *> *extensions = [NSMutableArray array];
  if (!filter)
    return extensions;

  const wchar_t *ptr = filter;
  while (*ptr != L'\0') {
    while (*ptr != L'\0')
      ptr++;

    ptr++;

    if (*ptr == L'\0')
      break;

    // Parse extensions
    NSString *extStr = WCharToNSString(ptr);
    NSArray *components =
        [extStr componentsSeparatedByCharactersInSet:
                    [NSCharacterSet characterSetWithCharactersInString:@";,"]];
    for (NSString *comp in components) {
      NSString *cleanComp =
          [comp stringByTrimmingCharactersInSet:[NSCharacterSet
                                                    whitespaceCharacterSet]];
      if ([cleanComp hasPrefix:@"*."]) {
        cleanComp = [cleanComp substringFromIndex:2];
      }
      if (![cleanComp isEqualToString:@"*"] && cleanComp.length > 0) {
        [extensions addObject:cleanComp];
      }
    }

    while (*ptr != L'\0')
      ptr++;

    ptr++;
  }
  return extensions;
}

std::optional<std::wstring> FileDialog::Open(const wchar_t *title,
                                             const wchar_t *filter) {
  @autoreleasepool {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setTitle:WCharToNSString(title)];
    [panel setCanChooseFiles:YES];
    [panel setCanChooseDirectories:NO];
    [panel setAllowsMultipleSelection:NO];

    NSMutableArray<NSString *> *exts = ParseFilter(filter);
    if (exts.count > 0) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
      [panel setAllowedFileTypes:exts];
#pragma clang diagnostic pop
    }

    if ([panel runModal] == NSModalResponseOK) {
      NSURL *url = [[panel URLs] firstObject];
      if (url) {
        return NSStringToWString([url path]);
      }
    }
  }
  return std::nullopt;
}

std::optional<std::vector<std::wstring>>
FileDialog::MultiOpen(const wchar_t *title, const wchar_t *filter) {
  @autoreleasepool {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setTitle:WCharToNSString(title)];
    [panel setCanChooseFiles:YES];
    [panel setCanChooseDirectories:NO];
    [panel setAllowsMultipleSelection:YES];

    NSMutableArray<NSString *> *exts = ParseFilter(filter);
    if (exts.count > 0) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
      [panel setAllowedFileTypes:exts];
#pragma clang diagnostic pop
    }

    if ([panel runModal] == NSModalResponseOK) {
      std::vector<std::wstring> files;
      for (NSURL *url in [panel URLs]) {
        files.push_back(NSStringToWString([url path]));
      }
      if (!files.empty()) {
        return files;
      }
    }
  }
  return std::nullopt;
}

std::optional<std::wstring> FileDialog::SaveAs(const wchar_t *title,
                                               const wchar_t *filter) {
  @autoreleasepool {
    NSSavePanel *panel = [NSSavePanel savePanel];
    [panel setTitle:WCharToNSString(title)];

    NSMutableArray<NSString *> *exts = ParseFilter(filter);
    if (exts.count > 0) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
      [panel setAllowedFileTypes:exts];
#pragma clang diagnostic pop
    }

    if ([panel runModal] == NSModalResponseOK) {
      NSURL *url = [panel URL];
      if (url) {
        return NSStringToWString([url path]);
      }
    }
  }
  return std::nullopt;
}

#endif // __APPLE__