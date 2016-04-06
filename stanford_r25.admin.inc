<?php

function _stanford_r25_credentials($form, &$form_state)
{

  // start with an instructive string
  $markup_str = 'Enter your credentials for the R25 API.<br /> Don\'t have credentials? File a HelpSU ticket to Administrative Applications/25 Live.';

  $credentials = explode(':', decrypt(variable_get('stanford_r25_credential', '')), 2);
  $clientid = $credentials[0];
  $password = (isset($credentials[1])) ? $credentials[1] : '';

  $base_url = variable_get('stanford_r25_base_url', 'https://webservices.collegenet.com/r25ws/wrd/stanford/run');

  // see if we can authenticate the current credentials and post a message if we can
  if (_stanford_r25_api_call('test', $base_url, $clientid . ':' . $password)) {
    $markup_str .= '<br /><br />Good news! Your credentials are set and valid and your site can currently connect to the R25 API.';
  }

  $form['description'] = array(
      '#markup' => t($markup_str),
  );
  $form['stanford_r25_clientid'] = array(
      '#type' => 'textfield',
      '#title' => t('Username:'),
      '#default_value' => $clientid,
      '#required' => TRUE,
  );

  $form['stanford_r25_password'] = array(
      '#type' => 'password',
      '#title' => t('Password:'),
      '#default_value' => $password,
      '#required' => TRUE,
  );

  $form['stanford_r25_base_url'] = array(
      '#type' => 'textfield',
      '#title' => t('Base URL'),
      '#description' => t('Base URL for R25 Webs Services calls.'),
      '#default_value' => $base_url,
      '#required' => TRUE,
  );

  $form['stanford_r25_base_image_url'] = array(
      '#type' => 'textfield',
      '#title' => t('Base Image URL'),
      '#description' => t('Base Image URL to retrieve room images without credentials.'),
      '#default_value' => variable_get('stanford_r25_base_image_url', 'https://25live.collegenet.com/25live/data/stanford/run'),
      '#required' => false,
  );

    $fullcalendar_str = '';
    if (libraries_get_path('fullcalendar') === false) {
        $fullcalendar_str = 'You do not have the FullCalendar library installed, so you will not be able to set up self-service bookings. You can download the library from http://fullcalendar.io.';
    } else {
        $fullcalendar_str = 'Woohoo! You have the FullCalendar library available, so you can set rooms up for self-service bookings!';
    }
    $form['fullcal_msg'] = array(
        '#markup' => '<p>'.t($fullcalendar_str).'</p>',
    );
    $qtip_str = '';
    if (libraries_get_path('jquery.qtip') === false) {
        $qtip_str = 'You do not have the qTip library installed, so we won\'t be able to display extra information about bookings on the FullCalendar display. You can download the library from  http://qtip2.com.';
    } else {
        $qtip_str = 'Yay! You have the qTip library installed, so we can display extra information about bookings on the FullCalendar display when events are clicked!';
    }
    $form['qtip_msg'] = array(
        '#markup' => '<p>'.t($qtip_str).'</p>',
    );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save settings'),
  );
  return $form;
}

function _stanford_r25_credentials_validate($form, &$form_state) {
  if (empty($form_state['values']['stanford_r25_clientid'])) {
    form_set_error('stanford_r25_clientid', t("Error. Username and/or Password are missing."));
  } else if (empty($form_state['values']['stanford_r25_password'])) {
    form_set_error('stanford_r25_password', t("Error. Username and/or Password are missing."));
  } else if (empty($form_state['values']['stanford_r25_password'])) {
    form_set_error('stanford_r25_password', t("Error. Username and/or Password are missing."));
  } else if (empty($form_state['values']['stanford_r25_base_url'])) {
    form_set_error('stanford_r25_base_url', t("Error. Base URL is missing."));
  } else {
    $username = $form_state['values']['stanford_r25_clientid'];
    $password = $form_state['values']['stanford_r25_password'];
    $base_url = $form_state['values']['stanford_r25_base_url'];
    if (!_stanford_r25_api_call('test',$base_url,$username.':'.$password)) {
      form_set_error('stanford_r25_password', t("Error. Can't connect to R25 API. Please check your username and password."));
    }
  }
}

function _stanford_r25_credentials_submit($form, &$form_state) {
  $credential = '';
  if (!empty($form_state['values']['stanford_r25_clientid'])) {
    $credential .= $form_state['values']['stanford_r25_clientid'];
  }
  $credential .= ':';
  if (!empty($form_state['values']['stanford_r25_password'])) {
    $credential .= $form_state['values']['stanford_r25_password'];
  }
  $credential = encrypt($credential);
  variable_set('stanford_r25_credential',$credential);
  $base_url = '';
  if (isset($form_state['values']['stanford_r25_base_url'])) {
    $base_url = $form_state['values']['stanford_r25_base_url'];
  }
  variable_set('stanford_r25_base_url',$base_url);
  if (isset($form_state['values']['stanford_r25_base_image_url'])) {
      variable_set('stanford_r25_base_image_url',$form_state['values']['stanford_r25_base_image_url']);
  }
  drupal_set_message(t('Credentials saved.'));
}

function _stanford_r25_settings($form, &$form_state) {
  $form['codes'] = array(
'#type' => 'fieldset',
      '#title' => '25Live Codes',
      '#description' => t('Various codes for your department that need to be sent along with room requests.'),
  );
  $form['codes']['stanford_r25_org_id'] = array(
    '#type' => 'textfield',
      '#title' => t('Organization ID'),
      '#default_value' => variable_get('stanford_r25_org_id'),
      '#required' => true,
  );
  $form['codes']['stanford_r25_event_type'] = array(
      '#type' => 'textfield',
      '#title' => t('Event Type Code'),
      '#default_value' => variable_get('stanford_r25_event_type'),
      '#required' => true,
  );
  $form['codes']['stanford_r25_parent_event_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Parent Event ID'),
      '#default_value' => variable_get('stanford_r25_parent_event_id'),
      '#required' => true,
  );
  $form['stanford_r25_login_msg'] = array(
      '#type' => 'textfield',
      '#title' => t('Anonymous User Login Message'),
      '#description' => t('Potentially helpful reminder for anonymous users that may need to log in before being able to reserve rooms.'),
      '#default_value' => variable_get('stanford_r25_login_msg','Reserve this room'),
  );
    $form['stanford_r25_login_uri'] = array(
      '#type' => 'textfield',
        '#title' => t('Login URL'),
        '#description' => t('Login URI for the reservation form if user is anonymous. Login message field must be set for this to show. Defaults to /user/login'),
        '#default_value' => variable_get('stanford_r25_login_uri','/user/login'),
    );
  $form['stanford_r25_notpermitted_msg'] = array(
      '#type' => 'textarea',
      '#title' => t('No Permission to Reserve Rooms Message'),
      '#description' => t('Informational message to logged in users without the "Book R25 Rooms" permission.'),
      '#default_value' => variable_get('stanford_r25_notpermitted_msg',''),
  );
  $form['stanford_r25_readonly_msg'] = array(
      '#type' => 'textarea',
      '#title' => t('Read Only Calendar Message'),
      '#description' => t('A message informing user that a room is not reservable.'),
      '#default_value' => variable_get('stanford_r25_readonly_msg',''),
  );
  $form['stanford_r25_booking_instructions'] = array(
      '#type' => 'textarea',
      '#title' => t('Booking Instructions'),
      '#description' => t('Instructions that will appear above room reservation forms.'),
      '#default_value' => variable_get('stanford_r25_booking_instructions',''),
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save settings'),
  );
  return $form;
}

function _stanford_r25_settings_validate($form, &$form_state) {
  if (empty($form_state['values']['stanford_r25_org_id']) || !is_numeric($form_state['values']['stanford_r25_org_id'])) {
    form_set_error('stanford_r25_org_id', t('Organization ID must be a number.'));
  }
  if (empty($form_state['values']['stanford_r25_event_type']) || !is_numeric($form_state['values']['stanford_r25_event_type'])) {
    form_set_error('stanford_r25_event_type', t('Event Type Code must be a number.'));
  }
  if (empty($form_state['values']['stanford_r25_parent_event_id']) || !is_numeric($form_state['values']['stanford_r25_parent_event_id'])) {
    form_set_error('stanford_r25_parent_event_id', t('Parent Event ID must be a number.'));
  }
}

function _stanford_r25_settings_submit($form, &$form_state) {
  if (isset($form_state['values']['stanford_r25_org_id'])) {
    variable_set('stanford_r25_org_id', $form_state['values']['stanford_r25_org_id']);
  }
  if (isset($form_state['values']['stanford_r25_event_type'])) {
    variable_set('stanford_r25_event_type', $form_state['values']['stanford_r25_event_type']);
  }
  if (isset($form_state['values']['stanford_r25_parent_event_id'])) {
    variable_set('stanford_r25_parent_event_id', $form_state['values']['stanford_r25_parent_event_id']);
  }
  if (isset($form_state['values']['stanford_r25_login_msg'])) {
    variable_set('stanford_r25_login_msg',$form_state['values']['stanford_r25_login_msg']);
  }
    if (isset($form_state['values']['stanford_r25_login_uri'])) {
        variable_set('stanford_r25_login_uri',$form_state['values']['stanford_r25_login_uri']);
    }
  if (isset($form_state['values']['stanford_r25_notpermitted_msg'])) {
    variable_set('stanford_r25_notpermitted_msg',$form_state['values']['stanford_r25_notpermitted_msg']);
  }
  if (isset($form_state['values']['stanford_r25_readonly_msg'])) {
    variable_set('stanford_r25_readonly_msg',$form_state['values']['stanford_r25_readonly_msg']);
  }
    if (isset($form_state['values']['stanford_r25_booking_instructions'])) {
        variable_set('stanford_r25_booking_instructions',$form_state['values']['stanford_r25_booking_instructions']);
    }
  drupal_set_message(t('Configuration saved.'));
}

function _stanford_r25_room_list() {
  $rooms = _stanford_r25_room_config_load();
  //$rooms = variable_get('stanford_r25_rooms',array());
  $header = array(
    t('Machine Name'),
    t('Display Name'),
      t('Calendar Type'),
    t('25Live Publisher Webname'),
    t('25Live Space ID'),
    t('Email Requests'),
    t('Created'),
    t('Status'),
    array('data' => t('Operations'), 'colspan' => '3'),
  );
  $rows = array();

  foreach ($rooms as $key => $config) {
    $row = array();
    $row[] = array('data' => l($key, '/r25/'.$key.'/calendar'));
    $row[] = array('data' => $config['display_name']);
      switch (intval($config['caltype'])) {
          case 1: $caltype = t('25Live Publisher'); break;
          case 2: $caltype = t('FullCalendar'); break;
          default: $caltype = t('Unknown'); break;
      }
      $row[] = array('data' => $caltype);
    $row[] = array('data' => $config['spud_name']);
    $row[] = array('data' => $config['space_id']);
    $row[] = array('data' => $config['email_list']);
    $row[] = array('data' => $config['created']);
    switch ($config['status']) {
      case 1: $status = t('Read-Only Display'); break;
      case 2: $status = t('Tentative Bookings'); break;
      case 3: $status = t('Confirmed Bookings'); break;
      default: $status = t('Disabled');
    }
    $row[] = array('data' => $status);

    // Set the edit column.
    $row[] = array('data' => l(t('edit'), STANFORD_R25_MENU_PATH . '/' . $key .'/edit'));

    // Set the delete column.
    $row[] = array('data' => l(t('delete'), STANFORD_R25_MENU_PATH . '/' . $key . '/delete'));

    $rows[] = $row;
  }
  $build['stanford_r25_rooms_list_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No room configurations are available. <a href="@link">Add a configuration</a>.', array('@link' => url(STANFORD_R25_MENU_PATH . '/list/add'))),
  );
  return $build;
}

function _stanford_r25_room_config($form, &$form_state, $roomid = NULL) {
  $form = array();
  $config = array();
  if (isset($roomid) && $roomid !== 'add') {
    $rooms = _stanford_r25_room_config_load();
    if (isset($rooms[$roomid])) {
      $config = $rooms[$roomid];
    }
  }
  if (empty($config)) {
    $config = array('machine_name'=> '', 'display_name'=>'', 'caltype'=>1, 'spud_name'=>'', 'space_id'=>'',
        'email_list'=>'', 'created' => format_date(time()), 'status'=>1, 'capacity'=>5);
  }
  $form['display_name'] = array(
    '#title' => t('Display Name'),
    '#type' => 'textfield',
    '#default_value' => $config['display_name'],
    '#description' => t('The human-readable name of the configuration.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  $form['machine_name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $config['machine_name'],
    '#maxlength' => 32,
    '#disabled' => !empty($config['machine_name']),
    '#machine_name' => array(
      'exists' => '_stanford_r25_room_config_exists',
      'source' => array('display_name'),
    ),
    '#description' => t('A unique machine-readable name for the configuration. It must only contain lowercase letters, numbers, and underscores.'),
  );
    $form['caltype'] = array(
        '#type' => 'radios',
        '#title' => t('Calendar Display Options'),
        '#default_value' => $config['caltype'],
        '#options' => array(1 => '25Live Publisher', 2 => 'FullCalendar'),
        '#description' => t('Whether to use the 25Live Publisher read-only calendar display or the interactive FullCalendar display.'),
    );
  $form['status'] = array(
      '#type' => 'radios',
      '#title' => t('Room Display Options'),
      '#default_value' => $config['status'],
      '#options' => array(0 => 'Disabled', 1 => 'Read-Only Calendar', 2 => 'Tentative Bookings', 3 => 'Confirmed Bookings'),
      '#description' => t('Whether to just display a calendar or allow tentative or confirmed bookings.'),
  );
  $form['spud_name'] = array(
    '#title' => t('25Live Publisher Webname'),
    '#type' => 'textfield',
    //'#reqired' => true,
    '#size' => 30,
    '#default_value' => $config['spud_name'],
    '#description' => t('The 25Live Publisher webname or "spud" name for this room\'s public calendar display. Required for 25Live Publisher display.'),
    '#states' => array('required' => array(
      ':input[name="caltype"]' => array('value'=>1)
    )),
  );
  $form['space_id'] = array(
      '#title' => t('R25 Room ID'),
      '#type' => 'textfield',
      '#reqired' => true,
      '#size' => 30,
      '#default_value' => $config['space_id'],
      '#description' => t('The R25 space_id code for this room. Required for tentative and confirmed bookings.'),
      '#states' => array('required' => array(
        ':input[name="status"]' => array(array('value'=>2),array('value'=>3))
      )),
  );
  $form['email_list'] = array(
      '#title' => t('Email List'),
      '#type' => 'textfield',
      '#size' => 30,
      '#default_value' => $config['email_list'],
      '#description' => t('Comma-separated list of email addresses which should receive notification of new reservation requests. Leave blank for "none".'),
  );
    $form['capacity'] = array(
      '#title'=>'Capacity',
        '#type'=> 'hidden',
        '#default_value' => $config['capacity'],
    );
  $form['created'] = array(
    '#title' => t('Created on'),
    '#type' => 'textfield',
    '#default_value' => $config['created'],
    '#disabled'=>true,
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#submit' => array('_stanford_r25_room_config_submit'),
    '#weight' => 40,
  );
  return $form;
}

function _stanford_r25_room_config_validate($form, &$form_state) {
  $status = 0;
  if (isset($form_state['values']['status'])) {
    $status = intval($form_state['values']['status']);
  }
    $caltype = 0;
    if (isset($form_state['values']['caltype'])) {
        $caltype = intval($form_state['values']['caltype']);
    }
   if (empty($form_state['values']['spud_name']) && $caltype == 1 && $status > 0) {
    form_set_error('spud_name','25Live Publisher webname is required to display calendar.');
  }
  if (empty($form_state['values']['space_id']) && $status > 1) {
    form_set_error('space_id','R25 Room ID is required to enable bookings.');
  }
}

function _stanford_r25_room_config_submit($form, &$form_state) {
  $fields = array(
    'machine_name' => (string) $form_state['values']['machine_name'],
    'display_name' => (string) $form_state['values']['display_name'],
    'spud_name' => (string) $form_state['values']['spud_name'],
    'space_id' => (string) $form_state['values']['space_id'],
    'email_list' => (string) $form_state['values']['email_list'],
    'created' => (string) $form_state['values']['created'],
    'status' => (int) $form_state['values']['status'],
      'caltype' => (int) $form_state['values']['caltype'],
      'capacity' => (int) $form_state['values']['capacity'],
  );
    // try and get room capacity
    if (!empty($form_state['values']['space_id'])) {
        $space_id = (string)$form_state['values']['space_id'];
        $results = _stanford_r25_api_call('roominfo', null, null, $space_id);
        if ($results) {
            if (!empty($results['index']['R25:LAYOUT_CAPACITY'][0])) {
                $fields['capacity'] = $results['vals'][$results['index']['R25:LAYOUT_CAPACITY'][0]]['value'];
            }
        }
    }
    _stanford_r25_room_config_save($fields);
    $form_state['redirect'] = STANFORD_R25_MENU_PATH . '/list';
}

function _stanford_r25_room_delete_submit($form, &$form_state) {
  _stanford_r25_room_config_delete_room($form_state['values']['machine_name']);
  $form_state['redirect'] = STANFORD_R25_MENU_PATH . '/list';
}

function _stanford_r25_room_delete($form, &$form_state, $config = null) {
  $rooms = _stanford_r25_room_config_load();
  $form = array('machine_name' => array('#type' => 'value', '#value' => $config),
                'label' => array('#type' => 'value', '#value' => $rooms[$config]['display_name']));
  $message = t('Are you sure you want to delete the configuration %label?', array('%label' => $rooms[$config]['display_name']));
  $caption = '<p>' . t('This action cannot be undone.') . '</p>';
  return confirm_form($form, filter_xss_admin($message), STANFORD_R25_MENU_PATH.'/list', filter_xss_admin($caption), t('Delete'));
}