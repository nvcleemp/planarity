# Introduction #

This page provides instructions for using Eclipse and the Subversive plugin to contribute to this project.

You must first follow the instructions in EclipseSetup.

# Details #

## Downloading the Trunk ##

A non-member is someone who does not have SVN commit access.  By clicking on the Source tab, you can get the link for read-only 'http' access to the Trunk.

A member is someone who does have SVN commit access granted by the administrator.  Once you are logged in, the Source tab will show the read-write 'https' link for the Trunk.

  1. **Obtain a local copy of the project trunk (or a branch)**
    1. Right-click Package Explorer
    1. New | Project
    1. SVN | Projects from SVN | Next >>
    1. URL: https://planarity.googlecode.com/svn/
      * Use just http for readonly download if you are a non-member
    1. User: same as your gmail login ID
      * If readonly access, then this won't be needed
    1. Password: Google code password, not your gmail login password
      * Log in to the Google Code Project
      * Under the source tab, there is a link to get your Google code password
      * If readonly access, then this won't be needed
    1. Click Save Password and then Click Next
    1. Next View is "Select Resource".  Use Browse... to choose the URL trunk, then click Finish
      * A branch or a changes directory could be selected here instead
    1. Next View is "Check out as".  Click Finish
      * This accepts "Check out as a project with the name specified" of Planarity-trunk, recursive checkout, no ignore

In a single workspace, the trunk and multiple branches, tags or changes directories can be selected as resources and checked out using the process above. Once an SVN project is checked out, the C project it contains must be imported into the workspace to enable compiling. Unfortunately only one C build project from one of the checked out resources can be in the workspace at a time, so you have to delete the C project of one SVN project resource before importing the C project of another.  Alternately, create multiple eclipse workspaces and check out one SVN project resource into each so that each workspace can import an actively compilable C project as follows:

  1. **Import the C project**
    1. Right-click package explorer and choose "Import..."
    1. Choose General | Existing Projects into Workspace
    1. Under "Select root directory" choose "Browse..."
    1. Navigate to the "c" directory under Planarity-trunk and hit OK
    1. Do not check "Copy projects into workspace"; just hit Finish
    1. This creates the Planarity-C project
  1. **First-time Build**
    1. Right-click Planarity-C project
    1. Choose "Build Configurations..." | Build | All and wait for builds to finish
    1. Right click "Debug" directory and choose Team | "Add to svn:ignore"
    1. Right click "Release" directory and choose Team | "Add to svn:ignore"
  1. **First-time Debug**
    1. Switch to the Debug Perspective
    1. From Toolbar, choose Run | "Debug Configurations..."
    1. Click C/C++ Local Application
    1. On the Main tab:
      1. Name: planarity.exe
      1. Project: Use "Browse..." to pick Planarity-c
      1. C/C++ App: Use "Search Projects..." to pick Debug\planarity.exe
    1. On the Debugger tab (Windows-only):
      1. Debugger: Choose "MinGW gdb Debugger"
      1. Optionallly, uncheck the "Stop on startup at main" option
      1. Debugger options: Use "Browse..." to select full path and name of gdb.exe
      1. Hit Apply to save this debug configuration
      1. Hit Debug to start debugging or Close to debug later

A member should also perform the steps below in order to be able to contribute changes for code review as described in the section below on "Contributing as a Member"

  1. **Make a "changes" directory for yourself**
    1. Open the SVN Repositories perspective
      * Window | Open Perspectives | Other...
      * SVN Repository Exploring
    1. Now make a code review directory for yourself in changes branch
      * Choose changes branch, then Right click and choose New | Folder
      * Use your gmail address for the folder name

## Contributing as a Non-Member ##

Since non-members don't have SVN commit access, branching is difficult.  However, this is not an impediment most of the time.  Work is often done against the trunk code, but code is not committed to the trunk without a code review.

Thus, the same basic instructions can be followed below for contributing as a member, except for how to request a code review.  A member would create a changes directory entry, copy the server-side trunk to it, create a patch based on the client-side trunk differences, and apply the patch to the copy of the server-side trunk in the changes directory.  A non-member would instead simply create a patch as a file and mail it to the contributor group for consideration.  A member would then create a changes directory entry, apply the patch from the non-member, and perform a code review.

## Contributing as a Member ##

  1. **When you want to make changes, work on trunk but DO NOT COMMIT**
    * To add a file, use Team | Add to Version Control... to add the file LOCALLY
    * To undo local changes to a file and go back to the server copy, use Team | Revert...
  1. **When you want a code review, you need to make a copy of your changes onto a copy of the trunk.**
    1. Copy the trunk to a directory in your changes location
      1. Go to SVN Repositories perspective
      1. Right Click on the trunk, choose Refactor | Copy To...
      1. Open the changes directory and click the subdirectory with your gmail name
      1. Rename the copy using a name associated with the feature your changes implement. For reference below, let the name be **FEATURE-X**
      1. Comment should just say that you are creating a copy to patch and review.
    1. Get your new copy into your project workspace
      1. Go back to Java Perspective and Right click Package Explorer
      1. New | Project
      1. SVN | Projects from SVN | Next >>
      1. The URL/user name/password dialog should be skipped this time
      1. Next View is "Select Resource"
        * Choose your new trunk copy named e.g. FEATURE-X from your changes directory
      1. Next View is "Check out as".  Click Finish
        * This accepts "Check out... Using New Project Wizard", recursive checkout, no ignore
      1. The "Select a wizard" dialog comes up
        1. Choose General | Project and hit next
        1. Project Name FEATURE-X
        1. Use default location and Click Finish
  1. **Apply your trunk changes to your FEATURE-X copy**
    1. Right click the planarity trunk project
    1. Team | Create Patch ...
    1. Hit Finish (accepts putting patch on the clipboard)
      * Note that changes to true binary files must be handled **manually** because the patch system only handles text
      * Note that .xhtml files are of type application/xhtml+xml, which is being treated as binary by SVN.  However, if the only binary files that were modified are really text files, it may be possible to patch them automatically by hitting Next instead of Finish in this step, and then selecting the "Process Binary Resources" checkbox
    1. Right click the FEATURE-X project
    1. Team | Apply Patch ...
    1. Hit Finish (pulls from clipboard)
    1. Right click the FEATURE-X project
    1. Team | Commit
    1. Add a comment that begins **FOR REVIEW** and explain feature
    1. After commit, you will be able to see a page for your revision
      * Go to planarity project and click the [sources tab](http://code.google.com/p/planarity/source/checkout)
      * Click the changes link, and at or near the top should be your revision (`http://code.google.com/p/planarity/source/detail?r=NNNN`)
      * The modifications can be viewed by expanding the trees OR to get a side-by-side diff comparison, don't click "Diff"! Instead, click the file link, then click "Diff" at the top right corner
    1. Do a non-rated review of your code.  In the general comments, put **CODE REVIEW NEEDED** and add any line-by-line comments that may be needed to help reviewers understand your code.
      * This will send an email to the planarity-eng@googlegroups.com with a subject that includes your **rNNNN** and a body that includes CODE REVIEW NEEDED, which can be used to filter emails.
      * The typical review period is 48 hours. If you need a shorter review period, send an email to planarity-eng@googlegroups.com with **URGENT** in the subject line and ask for quick reviews and also ask people to indicate if they don't intend to review (since you can't get a -1 from someone who doesn't review)
    1. At least one +1 review is needed before committing to trunk
      * A review can be +1 and can still request minor editorial changes needing no further review or identify issues that should be raised in the issue system once the code is committed.
      * A -1 review halts the commit until the reason for the -1 is resolved.  A -1 review requires a detailed technical justification, including what if anything can be done to mitigate or eliminate the concerns raised in the review.
      * A neutral review can be used to raise issues without advancing or impeding a trunk commit.
      * Ping list again if no +1 or -1 reviews within 48 hours
      * A +1, no -1's, and 48 hours are sufficient for a trunk commit
      * For URGENT requests, if a +1 is received, then ping the list to ask for a same day response to whether anyone else is planning a review.  If no response that day, then assume no further reviews are coming, and proceed with commit to trunk due to +1 received.
      * Except for addressing minor editorial changes, a substantive change should spawn a new review copy by repeating the above process, in which case the current review copy should be deleted without committing to trunk.
  1. **Committing changes to the trunk and deleting the review copy**
    1. Ensure you have the favorable review(s) needed
    1. Right click the trunk project and use Team | Update first
    1. Right click the trunk project and use Team | Commit...
    1. Delete FEATURE-X project from the package explorer
    1. Go to SVN Repositories perspective
    1. Right-click changes/FEATURE-X folder and choose Delete


## Working with a Tag, Branch or FEATURE-X Code Review Project ##

There is a limitation of creating only one Planarity-C project at a time in a workspace because Eclipse does not currently allow the imported project to be renamed.  So, while one can download the trunk as well as any number of tag, branch or FEATURE-X projects into the workspace simultaneously, the Planarity-C project from only one of them at a time can be imported into the workspace.

To download a tag, branch or code review project, the instructions are very similar to those given above for downloading the trunk.  The main difference is that the repository will already exist, so you won't have to type in the details nor give your user name and password again.  When presented with "Select Resource" dialog, choose the desired tag or branch rather than selecting trunk, and then hit Finish.  On the "Check out as..." dialog, provide an alternative descriptive name, such as Planarity-_tagname_, before clicking Finish.

Once this project is obtained, it is necessary to Import the C project in order to be able to compile and debug.  If the workspace already contains a Planarity-C project, then right click and Delete it.  Do not check the checkbox that asks to delete the files on disk.  Then, follow the same instructions as are given above for the trunk to import the C project, perform the first-time build, and perform first-time debug.

When you are done with this Planarity-C project, you can delete it.  Optionally, you can also delete the tag, branch or code review project.  Once done, you can then import the C project from the Planarity-trunk again, then perform the first-time build and first-time debug operations again.

## More about Working with a Branch ##

A branch is expected to allow mutations of the code relative to the baseline trunk code, unlike a tag or a FEATURE-X code review project, so it will likely be more convenient to have a separate workspace so that work on and compiling of the branch can continue in parallel with other branches and the trunk.  To begin a branch,

  1. right-click the trunk SVN project and choose Team | Branch...
  1. give a name Branch-FEATURE-X.
  1. create a new workspace in a directory such as Planarity-Branch-FEATURE-X
  1. follow the instructions above to "Obtain a local copy of the trunk (or a branch)" and "Import the C Project".

To ensure that your branch content does not drift to far from current Trunk content, periodically perform a merge of the trunk onto your branch.

  1. right-click the branch SVN project in the branch workspaces,
  1. choose  Team | Merge ...
  1. in the "One URL" tab, select the trunk, choose a Start Revision equal to when the branch was started or the revision after the last time you did the merge, and leave the Stop Revision at the Head of the trunk.

This will switch to the Team Synchronizing perspective and show you any changed files and any conflicts.  You can examine all of the merge results, but in particular you have to resolve any conflicts before you can continue.  For each resource showing a red conflict icon,

  1. double-click the file containing a conflict indicator
  1. search the local copy (on the left) for all occurrences of <<<<
  1. fix up the conflicts and remove the conflict markings.
  1. when you are done, save the file
  1. right-click the filename in the Team synchronizing view
  1. choose "Mark as merged".

When all conflicts are resolved,

  1. switch to the C/C++ perspective
  1. build and unit test the project
  1. Use Team | Commit... to commit the change to the branch
  1. Add the start and stop revision numbers of the trunk to the comment

Once work on a branch is finished,

  1. Go back to the workspace containing the trunk SVN project
  1. Use Team | Update ...
  1. Use Team | Merge ... to merge Branch-FEATURE-X onto the trunk local copy
    1. In the dialog, use the Reintegrate tab (Eclipse 3.5 and higher)
    1. For older Eclipse versions, you need to use the SVN command-line to reintegrate
  1. Follow the conflict resolution process above
  1. Your work is now not distinguishable from having been done on your trunk local copy the whole time
    1. Use your modified trunk as the basis of the Create | Patch ... step of the code review