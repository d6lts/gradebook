/* $Id $ */
Gradebook 2.0

Summary
-------

The Gradebook suite of modules provides taxonomy-based gradebooks for Drupal.
It consists of an API (gradebookapi.module) providing core gradebook
functionality, a basic user interface (gradebook.module) for sitewide
gradebooks, and several submodules that provide additional, optional features.
Its possible to build additional gradebook modules using the API with
or without gradebook.module. An example of a module that uses both is the
organic groups gradebook (og_gradebook) which provides a gradebook for each
organic group.

Students and teachers are identified by their Drupal user role. Teachers can
define gradebook categories (taxonomy terms) that can be unique for each 
gradebook. Gradebook assignments (and student responses when the gradebook
responses submodule is enabled) can be created using one or more content types.
Custom content types using CCK, or types supplied by other modules (such as
Webform and others) that will only be used as gradebook assignments are good
choices. Additional fields are added to assignment content types to allow the
teacher to select the gradebook category, the possible number of points for the
assignment, the due date for the assignment, and the date to publish the
assignment. Gradebook provides a gradebook page that lists all assignments that
have been published, and tabulates grades by student. Teachers enter grades into
the gradebook by clicking on cells within this table. This brings up an add grade
form that also allows the teachers to leave a text note, or exempt a student
from an assignment. The assignment titles in the gradebook page are linked to
the assignment node, so by clicking on the title, students and teachers can view
the assignments.  Students are also able to access the gradebook page where they
can view (but not edit!) their own grades, notes left by their teachers, and
up/download files (using the gradebook attachments submodule). Finally, each
gradebook can be exported and saved to a text or spreadsheet file.

This module is under active development, so feature requests are still possible
and would be appreciated. It is particularly important to run update.php when
upgrading from a previous version (or updating the 6.x.2.x-dev version).

I.   Installation
II.  Configuration and Customization
III. Usage
IV.  Summary of Optional Submodules
V.   Developer notes
VI.  Authors

I. Installation
---------------

1. Download the Gradebook module from the Drupal website
   (http://drupal.org/project/gradebook) and unpack it to the modules directory.
   You should now have modules/gradebook. There are actually several modules
   within this directory. Only gradebookapi and gradebook are needed for normal
   gradebook functionality. The others are optional submodules.
2. Go to the Administer >> Site Building >> Modules to enable gradebookapi,
   gradebook, and any submodules.
3. Follow normal administration procedures to run update.php.
4. That's it, you should now be able to use the Gradebook administration
   interface and can  enable Gradebook navigation menu items, if desired.

II. Configuration and Customization
-----------------------------------
The first step must be completed by the system administrator. After that the
designated gradebook administrator can take over, if desired.

1. If you haven't already done so, set up specific roles for the administrators,
   teachers and students who will be using the gradebook. This is done through
   the Drupal administration menu (User Management >> Roles).

   A. The Gradebook module provides 'administer gradebook' and 'access gradebook'
      permissions. The 'administer gradebook' permission is necessary to allow
      a user to add, edit, and view gradebooks from the administration page
      (admin/gradebook/gradebook). Its also needed for assigning gradebook
      students and teachers to the available Drupal roles, as well as for general
      gradebook settings. 'access gradebook' is needed to let a user role view
      the gradebook. This can be used to keep the gradebook hidden from
      anonymous, for example.

   B. The Gradebook API modeule provides 'administer gradebook api' permissions.
      This permissions allows the user to configure the gradebook module,
      determining what content types are treated as assignments.

   C. Here are the recommended permissions beyond the usual for authenticated
      users:
      i. Gradebook Administrators:
	  1. System module: 'access administration pages'
	  2. System module: 'administer site configuration'
	  3. Node Module:   create assignment content types
	  4. 'administer gradebook'
      ii. Teachers:
	  1. 'create ...' - at least the assignment types
	  2. 'access gradebook'
          3. The module will permit them to set up, modify, and add to
             gradebooks created by the Gradebook administrator according to
             their role.
     iii. Students:
	  1. 'access gradebook'
          2. The module will make sure they can only see their own grades and
             files intended for them.

   D. As usual, the system administrator with User Id = 1 can do anything.

2. Go to Administer >> Gradebook >> Gradebooks >> Settings
   (admin/gradebook/gradebook/settings) and check the roles that will be used
   for Teachers and Students. You can also designate general gradebook settings,
   including permissions for students and teachers to attach files to grades,
   the text to be used for an ungraded assignment (default is --), as well as
   the number of gradebooks to display on  the gradebook list page (default is
   25), and the number of grades to display on the gradebook page.

3. If you haven't already set up a new content type for assignments (and
   optionally student responses to  assignments), you probably will want to do
   that next. The Gradebook module will add new fields to assignment page. The
   teacher will be required to select the gradebook category that the assignment
   is associated with, the number of possible points for the assignment, when
   the assignment is due, when the assignment should be published, and which 
   gradebook scale to use for the assignment (if enabled).

   You can set up different assignment types or just have a single assignment
   type(s). Make sure teachers have permissions to create assignments, and
   students have permission to view them. Also, if using student responses
   students must be able to create whichever content type is being used for
   that purpose, and teachers must be able to view it.

4. Go to Administer >> Gradebook >> GradebookAPI to specify the content type(s)
   to use for assignments (and, optionally,  student responses). Checking these
   boxes will enable the form elements described above, so only select custom 
   content types that you have created for these purposes.

5. On the same page, choose an appropriate default time between creation of
   assignment node and due date. The actual due date will be selected on the 
   assignment form, but its easier to work with if the select form starts at
   a reasonable date.

6. Gradebook provides two blocks - Gradebook Legend, showing the background
   colors and text attributes used on the gradebook grade page, and
   My Assignments, listing all of a students current assignments for each
   gradebook. These are configured at Administer >> Site Building >> Blocks.

7. Gradebook and its submodules implement Views 2. At present there are no
   default views, but views can be designed to display gradebook data.

8. Gradebook and its submodules define themable functions that can be customized
   as desired. See http://drupal.org/theme-guide/6 for details on how to do this.

9. Gradebook and its submodules use default css stylesheets to control the page
   presentation. These default styles can be modified by the theme to customize
   the appearance of the gradebook. See http://drupal.org/node/263967 for
   details on how to do this.

III. Usage (for students and teachers)
--------------------------------------

1. A listing of all gradebooks is provided by the Gradebook link on the
   Navagation Menu.
   A. Note that this link is not enabled by default, so the system
      administrator must visit Administer >> Site Building >> Menus >> 
      Navigation to enable it.
2. Clicking on one of the listed gradebooks reveals a table of assignments
   (sorted by title, possible, category or date) with scores for each student.
   A. Students will only see their own name and grades listed.
   B. Teachers will see all the students in the gradebook. Clicking on a
      particular student name will filter the list to show assignments for one
      student.
3. From this location, teachers are allowed to:
   A. Create, edit, and view gradebook assignment categories (such as Exam,
      Quiz, Essay, etc).
      i. Categories can be nested by selecting one assignment category as the
         parent for another (so Quiz >> type1,  Quiz >> type2, etc.  are
         possible).
   B. View individual assignments by clicking on the assignment name
   C. Edit a student's grades (by clicking on the 'ungraded' text (--) in the
      grade list. [Note that, depending on your theme, sometimes it's hard to
      tell that this is a link.]
      i.   Assign a value for the student's score.
      ii.  Add a note that the student will be able to see when viewing their
           grades.
      iii. Exempt a student from the assignment, by checking the box. When this
           box is checked, the scores for the assignment are not used to
           calculate the student's grade.
      iv.  Click 'Save All' to save the grade and return to the gradebook page.
   D. Configure individual gradebook settings (such as default gradebook sort
      order) through the Settings tab. Submodules may add additional per-
      gradebook settings through this form.
4. If a student clicks on their grade, they are brought to a grade summary page,
   which includes the assignment data as well as any note provided by the
   teacher. A button at the bottom of this page brings the student back to the
   gradebook.
5. The teacher will also see [edit-all] in the table header under each
   assignment. This link brings up a grade entry form listing each student's
   grade data for the assignment. Changes will not be committed until the
   'Save All' button is pressed. A button is also provided to return to the
   gradebook page without saving.
6. The teacher can also view or edit all grade information for a particular
   student by clicking on the student's name.  This link brings up a grade entry
   form like that discussed in 5. Follow the same procedures to update all grade
   data for the student.
7. A themeable assignment notice is shown on assignment nodes when viewed by
   students and teachers of the class. The style of this notice can be adjusted
   by modifying the gradebook.css stylesheet. The content can also be modified
   (or eliminated) using a theme override.

IV. Description of Optional Submodules
--------------------------------------

1.  Gradebook Attachments allows teachers and/or users to attach files to grades.
    A. This module provides 'attach files to gradebook' permission. Any role
       granted this permission can attach files. Alternatively, it may be more
       convenient to grant this permission from the  sitewide Gradebook Settings
       page at Administer >> Gradebook >> Gradebooks >> Settings
       (admin/gradebook/gradebook/settings).
    B. To attach a file to one of the grades, first select the grade by clicking
       on the radio button in the left column. Note that if there is only one
       grade, radio buttons will not appear. Then, browse to find the file and
       finally press the attach button to add the file to the grade record.
    C. The file is not attached untile the 'Save All' button has been pressed.
    D. Files are saved in a gradebook_attachments subdirectory of the site files
       directory, according to gradebook id (taxonomy term id). For example: All
       files attached to gradebook with term id 66 can be found in 
       files/gradebook_attachments/66.
    E. Grades with attachments are emphasized on the gradebook page using a css
       style. The default emphasis can be adjusted by overriding or editing the
       gradebook_attachments.css file.
    F. Restrictions can be placed on student attachments. The default behavior
       for an individual gradebook is set through the settings tab on the
       gradebook page. Teachers can override this default behavior by setting the
       attachments restrictions on the node edit page when creating assignments.
       Options include: do not permit attachments after the due date, do not
       permit attachments after a grade has been given, and do not permit
       attachments (at all).
    G. NOTE: When the module is uninstalled, all gradebook attachments will be
       deleted.
    H. NOTE: When a student's account is deleted, all attachments to the
       student's grades will be deleted.
    I. SECURITY: To secure the gradebook_attachments subdirectory so that only
       the admin, teacher, and student have access to the files attached to a
       student's grade, do the following:
       i.  Create a .htaccess file in the gradebook_attachments subdirectory
           that contains the following:

           #
           # Apache/PHP/Drupal settings:
           #

           <IfModule mod_rewrite.c>
             RewriteEngine on
             RewriteBase /system/files/gradebook_attachments
             RewriteRule ^(.*)$ $1 [L,R=301]
           </IfModule>

       ii.  You may need to adjust the RewriteBase for your website
            configuration, for example, by adding a directory before /system/
            for a multisite configuration or a configuration where DRUPAL is
            installed in a subdirectory.
       iii. Next, assign 'access gradebook attachments folder' permission to any
            role that should have (restricted) access to the gradebook
            attachments folder.
       iv.  Now when a request is made for an attached file from the
            gradebook_attachments folder, only the user who attached the file,
            the student whose grade it is
            attached to, or the teacher will be granted access (admin always has
            access).
       v.   See http://drupal.org/node/540754 and/or 
            http://www.drupalcoder.com/node/406 for further information on this
            technique.

2.  Gradebook Responses can be enabled to assign a custom content type to allow
    students to respond to assignments.
    A. The gradebook administrator must identify student responses types
       admin/gradebook/gradebookapi.
    B. When teachers create assignments, they can choose how many responses are
       allowed for a given assignment. Enter 0 to not enforce a limit.
    C. When students create response pages, Gradebook_responses adds two form
       elements that lets the student indicate which assignment they are
       responding to, and whether the assignment is in progress or completed.
    D. Teachers can see any student responses in the gradebook. They are listed
       with links below the title on the grade form.
    E. When a grade is given for an assignment with a response, the state of the
       response node is changed to graded, and students cannot modify these form
       elements anymore. (I am still developing these ideas, so feedback / use
       cases are welcomed).
    F. When assigning a grade, teachers can check 'redo permitted' to allow a
       student to submit another response for the (graded) assignment. This will
       change the response status to REDO for each response submitted for the
       assignment. Unchecking the redo permitted box and submitting a grade will
       return the response status to graded.
    G. Restrictions can be placed on student rattachments. The default behavior
       is not to allow students to respond to an assignment after the due date.
       This default behavior can be changed for an individual gradebook through
       the settings tab on the gradebook page. Teachers can also override this
       default behavior for individual assignments by setting the response
       restrictions on the node edit page when creating assignments.
    H. Grades with responses are emphasized on the gradebook page using a css
       style. The default emphasis can be adjusted by overriding or editing the
       gradebook_responses.css file.

3.  Gradebook Scales and Weights allows teachers to define grading scales, i.e.
    letter grades mapped to numerical point ranges. Category weighting, in
    calculating the total grade, can also be done for each gradebook when this
    sub-module is enabled.
    A. Defining scales is done from tabs provided to teachers on the gradebook
       page.
       i.   Add a gradebook assignment scale (Add Scale), or edit an existing
            scale (List Scales).
       ii.  Go to the add scale tab to create a new scale for your gradebook.
       iii. Provide a name to identify the scale - this will only be seen by the
            teacher.
       iv.  Provide a list of marks to associate with the percentage of the
            possible points for an assignment.
            a. The Min and Max fields bracket the point range for the mark.
            b. The mark can be any sequence of up to 5 characters (So A, B+, B,
               B- ... or PASS, FAIL, etc.)
            c. When the percent grade [(earned/possible)*100] for a student grade
               falls within the specified range, the mark is displayed instead of
               the assigned points. This permits hybrid (numeric, letter) scales
               like 0 - 50% = FAIL, with points displayed above that.
            d. Add a range and a mark on each line of the form. The form defaults
               to 6 lines to start, if you need more, save the scale and then
               edit again. There will always be 6 blank rows to work with.
               [TODO: AJAX would be very good here].
            e. Min, Max and Mark must be filled in for the row to be saved.
       v.   If you have several gradebooks, there is an option to make a copy of
            the scale you create for your other gradebooks.
       vi.  Each assignment can have a different scale. Select the scale to use
            when you create the assignment.
    B. Category weights are assigned using the Weight Categories tab available to
       teachers from the gradebook page
       i. There are two weighting techniques to choose from:
          a. Weight the final grade by category percentages.
             - Use this to make a gradebook category count for a specific
               fraction of the grade (eg. Tests 30%...)
          b. Weight the final grade by category multiplier.
             - This option multiplies the points in a category by a given
               percentage.

4.  Gradebook Roster provides a simple roster for each gradebok. When this
    module is enabled, teachers will find a Roster link above the gradebook
    page that can be used to define and modify the roster.
    A. When the roster is initialized, all students and teachers will be added.
       i.  A display name can be chosen that will appear in place of the user
           name in the gradebook.
       ii. Initially the display name is the same as the user name, unless the
           RealName module is installed. If that module is enabled, then the
           users RealName will be used.
    B. Users can be deleted or hidden from the roster.
       i.  Deleting a user from the roster only affects the gradebook display.
           Existing gradebook information will be preserved, and the user can
           be added back to the roster at a later time.
       ii. Hiding a user keeps them in the roster, but while hidden they are not
           considered to be a student or teacher.
    C. Users can be assigned role of teacher, student or grading assistant.
       i. Grading assistants can do everything a teacher can do, except they
          cannot modify the roster.

4.  Gradebook Attendance provides a means for recording student attendance and
    provides an attendance page that is similar to the gradebook page. When this
    module is enabled, students and teachers will have an Attendance tab when
    viewing the gradebook page. Clicking on this tab will take them to the
    attendance page. Student's will only see their own attendance records.
    Teachers will see the records for the entire class and will be able to edit
    or create new attendance records. Also, if attendance has not already
    been recorded "Take Today's Attendance" reminder link will appear above the
    gradebook page that will take the teacher to a form where they can record
    attendance.
    A. The teacher can choose to give a student one of three marks: Present,
       Absent, or Tardy.
    B. Attendance records are limited to one per gradebook per calendar day.
    C. Attendance records for the entire class can be edited or deleted by
       following the "edit" or "delete" links on the gradebook attendance page.
    D. An individual student's attendance record can be changed for a given day
       by clicking on the attendance record for that day.
    E. An individual student's entire attendance record can be edited by
       clicking on the student's name on the attendance page.
    F. The attendance page can be exported to ascii text or Excel spreadsheet
       through the Export tab on the gradebook page.
    G. Teacher's can create an attendance assignment that will provide points
       for student's attendance when attendance is taken over a range of dates.
       This is done by checking the "attendance assignment" box on the form when
       creating an assignment. If this box is checked, then the points specified
       in the "Points if Present, Absent and Tardy" fields will be given
       whenever attendance is taken between the publish and due dates for the
       attendance assignment. Grading is automatic. The grades are also updated
       if the assignment is modified. A special (themeable) notice is also shown
       on the assignment node explaining how attendance is graded.
    H. Attendance records are emphasized on the attendance page using a css
       style. The default emphasis can be adjusted by overriding or editing the
       gradebook_attendance.css file.

V. Integration with other modules
   Aside from the optional submodules, Gradebook 2.0 integrates with other Drupal
   modules.
   A. Taxonomy
   B. Views
   C. Rules
   D. Quiz   

VI. Developer notes
------------------
1. The Gradebook module was written for Drupal 4.7 and 5 by Robert Wohleb and
   was ported to Drupal 6 by Michael Nichols.
2. The initial Google SOC research project page is at http://drupal.org/node/60031
3. Discussion and Development on Drupal Groups at http://groups.drupal.org/soc-gradebook
4. Drupal in Education on Drupal Groups at http://groups.drupal.org/drupal-education
5. Drupal in Education discussion of Gradebook at http://groups.drupal.org/node/6447

VI.  Authors
--------------
*  Michael Nichols (MGN@drupal)
