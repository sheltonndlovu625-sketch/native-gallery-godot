package org.godotengine.plugin.nativegallery;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.media.MediaScannerConnection;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import org.godotengine.godot.Godot;
import org.godotengine.godot.plugin.GodotPlugin;
import org.godotengine.godot.plugin.UsedByGodot;
import java.util.ArrayList;
import java.util.List;

public class NativeGalleryPlugin extends GodotPlugin {
    private static final int REQUEST_PICK_IMAGE = 1001;
    private static final int REQUEST_PICK_VIDEO = 1002;
    private static final int REQUEST_PICK_MULTIPLE = 1003;

    private String lastResult = null;
    private String[] lastResults = null;
    private boolean lastCancelled = false;
    private boolean hasResult = false;

    public NativeGalleryPlugin(Godot godot) {
        super(godot);
    }

    @NonNull
    @Override
    public String getPluginName() {
        return "NativeGalleryAndroid";
    }

    @UsedByGodot
    public void pickImage(boolean multiple) {
        clearResult();
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("image/*");
        if (multiple && Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
            intent.putExtra(Intent.EXTRA_ALLOW_MULTIPLE, true);
        }
        getActivity().startActivityForResult(
            Intent.createChooser(intent, "Select Image"),
            multiple ? REQUEST_PICK_MULTIPLE : REQUEST_PICK_IMAGE
        );
    }

    @UsedByGodot
    public void pickVideo(boolean multiple) {
        clearResult();
        Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
        intent.setType("video/*");
        if (multiple && Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
            intent.putExtra(Intent.EXTRA_ALLOW_MULTIPLE, true);
        }
        getActivity().startActivityForResult(
            Intent.createChooser(intent, "Select Video"),
            multiple ? REQUEST_PICK_MULTIPLE : REQUEST_PICK_VIDEO
        );
    }

    @UsedByGodot
    public void scanMedia(String path) {
        MediaScannerConnection.scanFile(
            getActivity().getApplicationContext(),
            new String[]{path},
            null,
            new MediaScannerConnection.OnScanCompletedListener() {
                @Override
                public void onScanCompleted(String scanPath, Uri uri) {
                }
            }
        );
    }

    @UsedByGodot
    public boolean hasResult() {
        return hasResult;
    }

    @UsedByGodot
    public String getLastResult() {
        hasResult = false;
        return lastResult;
    }

    @UsedByGodot
    public String[] getLastResults() {
        hasResult = false;
        return lastResults;
    }

    @UsedByGodot
    public boolean wasCancelled() {
        hasResult = false;
        return lastCancelled;
    }

    private void clearResult() {
        lastResult = null;
        lastResults = null;
        lastCancelled = false;
        hasResult = false;
    }

    @Override
    public void onMainActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode != REQUEST_PICK_IMAGE && requestCode != REQUEST_PICK_VIDEO && requestCode != REQUEST_PICK_MULTIPLE) {
            return;
        }
        if (resultCode == Activity.RESULT_OK && data != null) {
            if (data.getClipData() != null) {
                int count = data.getClipData().getItemCount();
                List<String> paths = new ArrayList<>();
                for (int i = 0; i < count; i++) {
                    Uri uri = data.getClipData().getItemAt(i).getUri();
                    paths.add(uri.toString());
                }
                lastResults = paths.toArray(new String[0]);
            } else if (data.getData() != null) {
                lastResult = data.getData().toString();
            }
        } else if (resultCode == Activity.RESULT_CANCELED) {
            lastCancelled = true;
        }
        hasResult = true;
    }
}
