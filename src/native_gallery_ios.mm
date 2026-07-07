#include "native_gallery.h"

#if defined(__APPLE__) && defined(TARGET_OS_IOS)

#import <UIKit/UIKit.h>
#import <PhotosUI/PhotosUI.h>
#import <Foundation/Foundation.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

using namespace godot;

@interface NativeGalleryIOSDelegate : NSObject <PHPickerViewControllerDelegate>
@property (nonatomic, assign) NativeGallery *gallery;
@property (nonatomic, assign) bool is_video;
@end

@implementation NativeGalleryIOSDelegate

- (void)picker:(PHPickerViewController *)picker didFinishPicking:(NSArray<PHPickerResult *> *)results API_AVAILABLE(ios(14.0)) {
    [picker dismissViewControllerAnimated:YES completion:nil];

    if (results.count == 0) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (self.gallery) {
                self.gallery->_on_cancelled();
            }
        });
        return;
    }

    if (self.gallery->allow_multiple) {
        __block NSMutableArray *pathStrings = [NSMutableArray array];
        dispatch_group_t group = dispatch_group_create();
        dispatch_queue_t arrayQueue = dispatch_queue_create("com.nativegallery.array", DISPATCH_QUEUE_SERIAL);

        for (PHPickerResult *result in results) {
            dispatch_group_enter(group);
            [self loadResult:result completion:^(NSString *path) {
                if (path) {
                    dispatch_async(arrayQueue, ^{
                        [pathStrings addObject:path];
                    });
                }
                dispatch_group_leave(group);
            }];
        }

        dispatch_group_notify(group, dispatch_get_main_queue(), ^{
            PackedStringArray paths;
            for (NSString *path in pathStrings) {
                paths.push_back(String([path UTF8String]));
            }
            if (self.gallery) {
                if (self.is_video) {
                    self.gallery->_on_videos_selected(paths);
                } else {
                    self.gallery->_on_images_selected(paths);
                }
            }
        });
    } else {
        PHPickerResult *result = results[0];
        [self loadResult:result completion:^(NSString *path) {
            if (path && self.gallery) {
                String godot_path = String([path UTF8String]);
                if (self.is_video) {
                    self.gallery->_on_video_selected(godot_path);
                } else {
                    self.gallery->_on_image_selected(godot_path);
                }
            }
        }];
    }
}

- (void)loadResult:(PHPickerResult *)result completion:(void (^)(NSString *))completion API_AVAILABLE(ios(14.0)) {
    NSItemProvider *provider = result.itemProvider;
    NSString *typeIdentifier = self.is_video ? @"public.movie" : @"public.image";

    if ([provider hasItemConformingToTypeIdentifier:typeIdentifier]) {
        [provider loadFileRepresentationForTypeIdentifier:typeIdentifier completionHandler:^(NSURL *url, NSError *error) {
            if (url && !error) {
                NSFileManager *fm = [NSFileManager defaultManager];
                NSString *tmpDir = NSTemporaryDirectory();
                NSString *fileName = [url lastPathComponent];
                if (!fileName || fileName.length == 0) {
                    fileName = self.is_video ? @"picked_video.mp4" : @"picked_image.png";
                }
                NSString *destPath = [tmpDir stringByAppendingPathComponent:fileName];
                NSURL *destURL = [NSURL fileURLWithPath:destPath];

                [fm removeItemAtURL:destURL error:nil];
                NSError *copyError = nil;
                [fm copyItemAtURL:url toURL:destURL error:&copyError];

                dispatch_async(dispatch_get_main_queue(), ^{
                    completion(copyError ? nil : destPath);
                });
            } else {
                dispatch_async(dispatch_get_main_queue(), ^{
                    completion(nil);
                });
            }
        }];
    } else {
        completion(nil);
    }
}

@end

void NativeGallery::_pick_native_image(bool p_multiple) {
    if (@available(iOS 14, *)) {
        PHPickerConfiguration *config = [[PHPickerConfiguration alloc] init];
        config.selectionLimit = p_multiple ? 0 : 1;
        config.filter = [PHPickerFilter imagesFilter];

        PHPickerViewController *picker = [[PHPickerViewController alloc] initWithConfiguration:config];
        NativeGalleryIOSDelegate *delegate = [[NativeGalleryIOSDelegate alloc] init];
        delegate.gallery = this;
        delegate.is_video = false;
        picker.delegate = delegate;

        UIViewController *rootVC = [UIApplication sharedApplication].keyWindow.rootViewController;
        if (rootVC) {
            [rootVC presentViewController:picker animated:YES completion:nil];
        } else {
            _on_error(2, "Could not get root view controller");
        }
    } else {
        _on_error(3, "iOS 14+ is required for PHPickerViewController");
    }
}

void NativeGallery::_pick_native_video(bool p_multiple) {
    if (@available(iOS 14, *)) {
        PHPickerConfiguration *config = [[PHPickerConfiguration alloc] init];
        config.selectionLimit = p_multiple ? 0 : 1;
        config.filter = [PHPickerFilter videosFilter];

        PHPickerViewController *picker = [[PHPickerViewController alloc] initWithConfiguration:config];
        NativeGalleryIOSDelegate *delegate = [[NativeGalleryIOSDelegate alloc] init];
        delegate.gallery = this;
        delegate.is_video = true;
        picker.delegate = delegate;

        UIViewController *rootVC = [UIApplication sharedApplication].keyWindow.rootViewController;
        if (rootVC) {
            [rootVC presentViewController:picker animated:YES completion:nil];
        } else {
            _on_error(2, "Could not get root view controller");
        }
    } else {
        _on_error(3, "iOS 14+ is required for PHPickerViewController");
    }
}

void NativeGallery::_scan_media(const String &p_path) {
    UtilityFunctions::print("NativeGallery: scan_media is a no-op on iOS (path: ", p_path, ")");
}

void NativeGallery::_poll_android() {
    // No-op on iOS
}

#endif // TARGET_OS_IOS
