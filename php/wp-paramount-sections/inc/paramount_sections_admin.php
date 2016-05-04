<?php
class paramount_sections extends WP_Widget {

    /**
     * Register the widget with WordPress.
     */
    function __construct() {
        $widget_ops = array( 'description' => 'A widget to handle important custom sections.' );
        parent::__construct(
            'paramount_sections',   // Base ID
            __('Paramount Sections', 'nlc_custom'), // Name of widget
            $widget_ops
        );
        add_action( 'wp_head', array(&$this, 'import_styles'), 10, 1 );
        add_action( 'init', array(&$this, 'initialize_widget' ) );
        add_action( 'wp_ajax_ps_block_ps_add_new', array($this, 'add_column') );
    }

    /*
     * Initializer method for widget.
     */
    function initialize_widget() {
        if ( version_compare( get_bloginfo( 'version' ), '3.0', '<' ) && is_ssl() ) {
            $wp_content_url = str_replace( 'http://', 'https://', get_option( 'siteurl' ) );
        } else {
            $wp_content_url = get_option( 'siteurl' );
        }
        $wp_content_url .= '/wp-content';
        $wp_plugin_url = $wp_content_url . '/plugins';

        // Register the scripts and styles needed for the widget in the admin widgets page
        wp_register_style('paramount_sections_css', $wp_plugin_url . '/paramount-sections/css/admin.css');
        wp_enqueue_style('paramount_sections_css');

        wp_register_script('paramount_sections_js', $wp_plugin_url . '/paramount-sections/js/jquery.paramountsections.0.1.js', array( 'jquery', 'jquery-ui-core', 'jquery-ui-sortable'), '1.0.0', false);
        wp_enqueue_script('paramount_sections_js');
    }

    /**
     * Method to import the front-end styles
     */
    function import_styles() {
        if ( !is_admin() ) {
            $url = plugins_url('../css/psmain.css', __FILE__);
            wp_enqueue_style('ps-main-styles', $url);
        }
    }

    /**
     * Front-end display of widget.
     *
     * @see WP_Widget::widget()
     *
     * @param array $args      Widget arguments.
     * @param array $instance  Saved values from database.
     */
    public function widget( $args, $instance ) {
        // Extract the arguments from the widget arguments
        // Sample:
        /*
        (
            [name] => Homepage Content Section 1
            [id] => homepage-content-section-one
            [description] => Widgets in this area will be shown underneath the hero section.
            [class] =>
            [before_widget] => <aside id="faq_widget-4" class="widget widget_faq_widget left">
            [after_widget] => </aside>
            [before_title] => <h5>
            [after_title] => </h5>
            [widget_id] => faq_widget-4
            [widget_name] => CT FAQ
        )
        */
				error_log("Args:");
				error_log(print_r($args, true));
				error_log("Instance:");
				error_log(print_r($instance, true));

        extract( $args );
        extract( $instance );

        $num_columns = $instance['num_columns'];	// How many columns per row there should be

        // Display the container for the section
        echo $before_widget;
        ?>
        <div class="row">
        	<?php
        		$ps_column_str = "";

        		foreach( $columns as $col ) {
        			error_log("Column Type: " . $col['column_type']);
        			switch ( $col['column_type'] ) {
        				case 'standardcolumn':
		        			// Split the classes for the column using a comma delimiter
		        			$cur_css_classes = split(",", $col['css_classes']);
		        			// Replace any extraneous whitespace with no spaces
		        			$cur_css_classes = preg_replace('/\s+/', "", $cur_css_classes);
		        			// Join the array with spaces
		        			$cur_css_classes = implode(" ", $cur_css_classes);

		        			error_log("Classes:");
		        			error_log(print_r($cur_css_classes, true));

		        			$ps_column_str .= '<div class="ps-column standard-column-section col-xs-12 col-sm-' . $col['column_size'] . '">';
		        			$ps_column_str .= '		<span class="ps-column-icon ' . $cur_css_classes . '"></span>';
		        			$ps_column_str .= '		<h3 class="ps-column-header">' . $col['header'] . '</h3>';
		        			if ( trim( $col['content'] ) !== "" )
		        				$ps_column_str .= '		<p class="ps-column-content">' . $col['content'] . '</p>';
		        			$ps_column_str .= '		<a href="' . $col['button_link'] . '" class="btn btn-white ps-column-button">' . $col['button_text'] . "</a>";
		        			$ps_column_str .= '</div>';

		        			//echo $ps_column_str;
	        				break;
	        			case 'simpleimage':
	        				$ps_column_str .= '<div class="ps-column simple-image-section col-xs-12 col-sm-' . $col['column_size'] . '">';
	        				$ps_column_str .= '	<img class="custom-img-responsive" src="' . $col['simple_image_url'] . '" />';
	        				$ps_column_str .= '</div>';
	        				break;
	        			case 'imagewithcontent':
		        			// Split the classes for the column using a comma delimiter
		        			$cur_icon_classes = split(",", $col['ic_css_classes']);
		        			// Replace any extraneous whitespace with no spaces
		        			$cur_icon_classes = preg_replace('/\s+/', "", $cur_icon_classes);
		        			// Join the array with spaces
		        			$cur_icon_classes = implode(" ", $cur_icon_classes);

		        			$ps_column_str .= '<div class="ps-column image-with-content-section col-xs-12 col-sm-' . $col['column_size'] . '" style="background-image:url(' . $col['ic_background_image_url']  . '); background-size: cover; background-position: center; background-repeat: no-repeat;">';
		        			//$ps_column_str .= '<img class="ps-column-bg-img" src="' . $col['ic_background_image_url'] . '" />';
		        			$ps_column_str .= '	<div class="ps-column-text-container">';
		        			// $ps_column_str .= '		<span class="ps-column-icon ' . $cur_icon_classes . '"></span>';
		        			if ( trim( $col['ic_icon_url'] ) !== "" )
		        				$ps_column_str .= '		<img src="' . $col['ic_icon_url'] . '" />';
		        			if ( trim( $col['ic_header'] ) !== "" )
		        				$ps_column_str .= '		<h2 class="ps-column-header ps-column-h2">' . $col['ic_header'] . '</h2>';
		        			if ( trim( $col['ic_content'] ) !== "" )
		        				$ps_column_str .= '		<p class="ps-column-content">' . $col['ic_content'] . '</p>';
		        			if ( trim( $col['ic_button_link'] ) !== "" && trim( $col['ic_button_text'] ) !== "" )
		        				$ps_column_str .= '		<a href="' . $col['ic_button_link'] . '" class="btn btn-white ps-column-button">' . $col['ic_button_text'] . "</a>";
		        			$ps_column_str .= '	</div>';
		        			$ps_column_str .= '</div>';

		        			// echo $ps_column_str;
	        				break;
	        			case 'blockquote':
	        				$ps_column_str .= '<div class="ps-column blockquote-section col-xs-12 col-sm-' . $col['column_size'] . '">';
	        				if ( trim( $col['blockquote_icon_url'] ) !== "" && trim( $col['blockquote_css_classes'] ) === "" )
	        					$ps_column_str .= '<img src="' . $col['blockquote_icon_url'] . '" />';
	        				if ( trim( $col['blockquote_css_classes'] ) !== "" && trim( $col['blockquote_icon_url'] ) === "" ) {
	        					// Split the classes for the column using a comma delimiter
			        			$cur_css_classes = split(",", $col['blockquote_css_classes']);
			        			// Replace any extraneous whitespace with no spaces
			        			$cur_css_classes = preg_replace('/\s+/', "", $cur_css_classes);
			        			// Join the array with spaces
			        			$cur_css_classes = implode(" ", $cur_css_classes);
	        					$ps_column_str .= '		<span class="ps-column-icon ' . $cur_css_classes . '"></span>';
	        				}

	        				if ( trim( $col['blockquote_quote'] ) !== "" )
		        				$ps_column_str .= '	<p class="ps-column-content">' . $col['blockquote_quote'] . '</p>';

		        			$ps_column_str .= '	<a href="' . $col['blockquote_button_link'] . '" class="btn btn-blue ps-column-button">' . $col['blockquote_button_text'] . "</a>";
		        			$ps_column_str .= '</div>';

	        				break;
	        			case 'form':

	        				$ps_column_str .= '<div class="ps-column form-section col-xs-12 col-sm-' . $col['column_size'] . '">';
	        				$ps_column_str .= '	<div class="ps-column-header-container">';
	        				$ps_column_str .= '		<hr />';
	        				$ps_column_str .= '		<h2 class="ps-column-header ps-column-h2">' . $col['form_header'] . '</h2>';
	        				$ps_column_str .= '		<hr />';
		        			$ps_column_str .= '	</div>';
		        			$ps_column_str .= '	<div class="col-xs-12">';
		        			$ps_column_str .= '		<div class="col-xs-4 form-icon-container">';
		        			$ps_column_str .= '			<img src="' . $col['form_icon_one_url'] . '" />';
		        			$ps_column_str .= '			<span>' . $col['form_icon_one_text'] . '</span>';
		        			$ps_column_str .= '		</div>';
		        			$ps_column_str .= '		<div class="col-xs-4 form-icon-container">';
		        			$ps_column_str .= '			<img src="' . $col['form_icon_two_url'] . '" />';
		        			$ps_column_str .= '			<span>' . $col['form_icon_two_text'] . '</span>';
		        			$ps_column_str .= '		</div>';
		        			$ps_column_str .= '		<div class="col-xs-4 form-icon-container">';
		        			$ps_column_str .= '			<img src="' . $col['form_icon_three_url'] . '" />';
		        			$ps_column_str .= '			<span>' . $col['form_icon_three_text'] . '</span>';
		        			$ps_column_str .= '		</div>';
		        			$ps_column_str .= '	</div>';
		        			$ps_column_str .= '	<form class="ps-column-form" action="" method="POST">';
		        			$ps_column_str .= '		<input type="text" placeholder="' . $col['form_field_place_one'] . '" class="ps-column-form-textfield" name="' . $col['form_field_place_one'] . '" />';
		        			$ps_column_str .= '		<input type="text" placeholder="' . $col['form_field_place_two'] . '" class="ps-column-form-textfield" name="' . $col['form_field_place_two'] . '" />';
		        			$ps_column_str .= '		<textarea cols="40" rows="3" class="ps-column-form-textarea" name="' . $col['form_field_place_three'] . '" placeholder="' . $col['form_field_place_three'] . '" ></textarea>';
		        			$ps_column_str .= '	</form>';
		        			$ps_column_str .= '	<a href="' . $col['form_button_link'] . '" class="btn btn-blue ps-column-button">' . $col['form_button_text'] . "</a>";
		        			$ps_column_str .= '</div>';

		        			//echo $ps_column_str;
	        				break;
	        			default:
	        				break;
        			}
        		}
        		error_log($ps_column_str);
        		echo $ps_column_str;
        	?>
        </div>
        <?php echo $after_widget; ?>
    <?php
    }

    /**
     * Back-end widget form.
     *
     * @see WP_Widget::form()
     *
     * @param array $instance Previously saved values from database.
     */
    public function form( $instance ) {
        $defaults = array(
        	'columns' => array(
        			1 => array(
        				'column_title' => 'My Column',
               	'column_size' => 4,
               	'column_type' => 'standardcolumn',
        				'icon_url' => '',
        				'background_image_url' => '',
        				'header' => 'Default Header',
        				'content' => 'Short blurb below.',
        				'button_text' => 'Learn More',
        				'button_link' => '#',
        				'css_classes' => '',
        				'ic_icon_url' => '',
						'ic_background_image_url' => '',
						'ic_header' => 'Default Header',
						'ic_content' => 'Short blurb below.',
						'ic_button_text' => 'Learn More',
						'ic_button_link' => '#',
						'ic_css_classes' => '',
						'form_header' => 'Default Header',
						'form_icon_one_url' => '',
						'form_icon_one_css' => '',
						'form_icon_one_text' => 'Text One',
						'form_icon_two_url' => '',
						'form_icon_two_css' => '',
						'form_icon_two_text' => 'Text Two',
						'form_icon_three_url' => '',
						'form_icon_three_css' => '',
						'form_icon_three_text' => 'Text Three',
						'form_field_place_one' => 'Name',
						'form_field_place_two' => 'Email',
						'form_field_place_three' => 'Goals',
						'form_button_text' => 'Learn More',
						'form_button_link' => '#',
						'blockquote_icon_url' => '',
						'blockquote_css_classes' => '',
						'blockquote_background_image_url' => '',
						'blockquote_quote' => '',
						'blockquote_button_text' => 'Button Text',
						'blockquote_button_link' => '#',
						'simple_image_url' => ''
        			)
        		),
        	'num_columns' => 3
				);

				// Merge the defaults and the instance variables
				$instance = wp_parse_args( $instance, $defaults );

				// error_log(print_r($instance, true));

				// Extract breaks up the argument variable into separate variables
				extract( $instance );

				?>
				<div class="description clearfix">
					<p>
						<label for="<?php echo $this->get_field_id('num_columns'); ?>"><?php _e('Number of columns per row:') ?></label>
						<input type="text" class="widefat" id="<?php echo $this->get_field_id('num_columns'); ?>" name="<?php echo $this->get_field_name('num_columns'); ?>" value="<?php echo esc_attr($num_columns); ?>">
					</p>
					<ul class="ps-sortable-list clearfix" rel="<?php echo $column_id; ?>">
					<?php
						$columns = is_array($columns) ? $columns : $defaults['columns'];
						$count = 1;
						// Loop through each column - each column has the defaults listed above
						foreach( $columns as $col ) {
							$this->create_column( $col, $count );
							$count++;
						}
					?>
					</ul>
					<p></p>
						<a href="#" rel="ps" class="ps-sortable-add-new button">Add New</a>
					<p></p>
				</div>

			<?php
    }


    /**
     * Create a new column with an icon, image, header, subcontent, etc.
     */
    function create_column( $column = array(), $count = 0 ) {

    	$defaults = array(
	    		'column_title' => 'My Column',
	         'column_size' => 4,
	         'column_type' => 'standardcolumn',
				'icon_url' => '',
				'background_image_url' => '',
				'header' => 'Default Header',
				'content' => 'Short blurb below.',
				'button_text' => 'Learn More',
				'button_link' => '#',
				'css_classes' => '',
				'ic_icon_url' => '',
				'ic_background_image_url' => '',
				'ic_header' => 'Default Header',
				'ic_content' => 'Short blurb below.',
				'ic_button_text' => 'Learn More',
				'ic_button_link' => '#',
				'ic_css_classes' => '',
				'form_header' => 'Default Header',
				'form_icon_one_url' => '',
				'form_icon_one_css' => '',
				'form_icon_one_text' => 'Text One',
				'form_icon_two_url' => '',
				'form_icon_two_css' => '',
				'form_icon_two_text' => 'Text Two',
				'form_icon_three_url' => '',
				'form_icon_three_css' => '',
				'form_icon_three_text' => 'Text Three',
				'form_field_place_one' => 'Name',
				'form_field_place_two' => 'Email',
				'form_field_place_three' => 'Goals',
				'form_button_text' => 'Learn More',
				'form_button_link' => '#',
				'blockquote_icon_url' => '',
				'blockquote_css_classes' => '',
				'blockquote_background_image_url' => '',
				'blockquote_quote' => '',
				'blockquote_button_text' => 'Button Text',
				'blockquote_button_link' => '#',
				'simple_image_url' => ''
			);

			// Merge the defaults with the column information supplied by the user
			$column = wp_parse_args( $column, $defaults );

			// error_log(print_r($column, true));

			// Break the $column array into it's separate entities
			// extract( $column );

			?>

			<li id="<?php echo $this->get_field_id('column'); ?>-sortable-item<?php echo $count ?>" class="sortable-item" rel="<?php echo $count ?>">
				<div class="sortable-head clearfix">
					<div class="sortable-title">
						<strong><?php echo $column['column_title'] ?></strong>
					</div>
					<div class="sortable-handle">
						<a href="#">Open / Close</a>
					</div>
				</div>

				<div class="sortable-body">
					<p class="ps-standard-row">
						<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-column_title">
							Column Title<br/>
							<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-column_title" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][column_title]" value="<?php echo $column['column_title']; ?>" />
						</label>
					</p>
					<p class="ps-standard-row">
						<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-column_size">
							Column Size (based on Bootstrap 3 12-column grid)<br/>
							<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-column_size" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][column_size]" value="<?php echo $column['column_size']; ?>" />
						</label>
					</p>
					<p class="ps-standard-row">
						<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-column_type">
							Column Type<br/>
							<select id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-column_type" class="input-full column-type-dd" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][column_type]" class="widefat" style="width: 100%;">
            				<option value="standardcolumn" <?php selected($column['column_type'], 'standardcolumn'); ?>>
            					Standard Column
          					</option>
          					<option value="simpleimage" <?php selected($column['column_type'], 'simpleimage'); ?>>
          						Image
          					</option>
            				<option value="imagewithcontent" <?php selected($column['column_type'], 'imagewithcontent'); ?>>
            					Image with Content Overlayed
          					</option>
          					<option value="blockquote" <?php selected($column['column_type'], 'blockquote'); ?>>
            					Blockquote
          					</option>
          					<option value="form" <?php selected($column['column_type'], 'form'); ?>>
            					Form
          					</option>
            			</select>
						</label>
					</p>
					<div class="<?php echo ($column['column_type'] == 'standardcolumn') ? 'standard-column-section' : 'standard-column-section ps-hidden-sections' ?>">
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-icon_url">
								Icon URL<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-icon_url" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][icon_url]" value="<?php echo $column['icon_url']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-css_classes">
								Icon CSS Class (if using a font icon rather than an image)<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-css_classes" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][css_classes]" value="<?php echo $column['css_classes']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-background_image">
								Background Image URL<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-background_image" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][background_image_url]" value="<?php echo $column['background_image_url']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-header">
								Header<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-header" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][header]" value="<?php echo $column['header']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-content">
								Content Text<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-content" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][content]" value="<?php echo $column['content']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-button_text">
								Button Text<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-button_text" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][button_text]" value="<?php echo $column['button_text']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-button_link">
								Button Link<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-button_link" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][button_link]" value="<?php echo $column['button_link']; ?>" />
							</label>
						</p>
					</div>
					<!-- Begin Image with content overlayed section -->
					<div class="<?php echo ($column['column_type'] == 'imagewithcontent') ? 'image-with-content-section' : 'image-with-content-section ps-hidden-sections' ?>">
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-ic_icon_url">
								Icon URL<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-ic_icon_url" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][ic_icon_url]" value="<?php echo $column['ic_icon_url']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-ic_css_classes">
								Icon CSS Class (if using a font icon rather than an image)<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-ic_css_classes" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][ic_css_classes]" value="<?php echo $column['ic_css_classes']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-ic_background_image">
								Background Image URL<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-ic_background_image" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][ic_background_image_url]" value="<?php echo $column['ic_background_image_url']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-ic_header">
								Header<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-ic_header" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][ic_header]" value="<?php echo $column['ic_header']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-ic_content">
								Content Text<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-ic_content" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][ic_content]" value="<?php echo $column['ic_content']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-ic_button_text">
								Button Text<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-ic_button_text" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][ic_button_text]" value="<?php echo $column['ic_button_text']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-ic_button_link">
								Button Link<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-ic_button_link" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][ic_button_link]" value="<?php echo $column['ic_button_link']; ?>" />
							</label>
						</p>
					</div>
					<!-- Begin Form section -->
					<div class="<?php echo ($column['column_type'] == 'form') ? 'form-section' : 'form-section ps-hidden-sections' ?>">
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_header">
								Header<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_header" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_header]" value="<?php echo $column['form_header']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_one_url">
								Icon One URL<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_one_url" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_icon_one_url]" value="<?php echo $column['form_icon_one_url']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_one_css">
								Icon One CSS Class<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_one_css" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_icon_one_css]" value="<?php echo $column['form_icon_one_css']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_one_text">
								Icon One Text<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_one_text" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_icon_one_text]" value="<?php echo $column['form_icon_one_text']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_two_url">
								Icon Two URL<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_two_url" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_icon_two_url]" value="<?php echo $column['form_icon_two_url']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_two_css">
								Icon Two CSS Class<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_two_css" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_icon_two_css]" value="<?php echo $column['form_icon_two_css']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_two_text">
								Icon Two Text<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_two_text" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_icon_two_text]" value="<?php echo $column['form_icon_two_text']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_three_url">
								Icon Three URL<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_three_url" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_icon_three_url]" value="<?php echo $column['form_icon_three_url']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_three_css">
								Icon Three CSS Class<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_three_css" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_icon_three_css]" value="<?php echo $column['form_icon_three_css']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_three_text">
								Icon Three Text<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_icon_three_text" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_icon_three_text]" value="<?php echo $column['form_icon_three_text']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_field_place_one">
								Form Field Placeholder Onebr/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_field_place_one" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_field_place_one]" value="<?php echo $column['form_field_place_one']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_field_place_two">
								Form Field Placeholder Two<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_field_place_two" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_field_place_two]" value="<?php echo $column['form_field_place_two']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_field_place_three">
								Form Field Placeholder Three<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_field_place_three" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_field_place_three]" value="<?php echo $column['form_field_place_three']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_button_text">
								Form Button Text<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_button_text" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_button_text]" value="<?php echo $column['form_button_text']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_button_link">
								Form Button Link<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-form_button_link" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][form_button_link]" value="<?php echo $column['form_button_link']; ?>" />
							</label>
						</p>
					</div>
					<!-- Begin Blockquote section -->
					<div class="<?php echo ($column['column_type'] == 'blockquote') ? 'blockquote-section' : 'blockquote-section ps-hidden-sections' ?>">
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-blockquote_icon_url">
								Icon URL<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-blockquote_icon_url" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][blockquote_icon_url]" value="<?php echo $column['blockquote_icon_url']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-blockquote_css_classes">
								Icon CSS Class (if using a font icon rather than an image)<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-blockquote_css_classes" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][blockquote_css_classes]" value="<?php echo $column['blockquote_css_classes']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-blockquote_background_image">
								Background Image URL<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-blockquote_background_image" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][blockquote_background_image_url]" value="<?php echo $column['blockquote_background_image_url']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-blockquote_quote">
								Quote<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-blockquote_quote" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][blockquote_quote]" value="<?php echo $column['blockquote_quote']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-blockquote_button_text">
								Button Text<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-blockquote_button_text" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][blockquote_button_text]" value="<?php echo $column['blockquote_button_text']; ?>" />
							</label>
						</p>
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-blockquote_button_link">
								Button Link<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-blockquote_button_link" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][blockquote_button_link]" value="<?php echo $column['blockquote_button_link']; ?>" />
							</label>
						</p>
					</div>
					<!-- Begin Simple Image section -->
					<div class="<?php echo ($column['column_type'] == 'simpleimage') ? 'simple-image-section' : 'simple-image-section ps-hidden-sections' ?>">
						<p class="ps-standard-row">
							<label for="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-simple_image">
								Background Image URL<br/>
								<input type="text" id="<?php echo $this->get_field_id('columns'); ?>-<?php echo $count; ?>-simple_image" class="input-full" name="<?php echo $this->get_field_name('columns') ?>[<?php echo $count ?>][simple_image_url]" value="<?php echo $column['simple_image_url']; ?>" />
							</label>
						</p>
					</div>
					<p class="ps-standard-row"><a href="#" class="sortable-delete">Delete</a></p>
				</div>
			</li>


			<?php
    }

    function add_column() {
    	$count = isset( $_POST['count']) ? absint( $_POST['count']) : false;
    	$this->column_id = isset( $_POST['column_id']) ? $_POST['column_id'] : 'ps-column-9999';

    	// Default key/value for the tab
    	$column = array(
    		'column_title' => 'My Column',
            'column_size' => 4,
            'column_type' => 'standardcolumn',
				'icon_url' => '',
				'background_image_url' => '',
				'header' => 'Default Header',
				'content' => 'Short blurb below.',
				'button_text' => 'Learn More',
				'button_link' => '#',
				'css_classes' => '',
				'ic_icon_url' => '',
				'ic_background_image_url' => '',
				'ic_header' => 'Default Header',
				'ic_content' => 'Short blurb below.',
				'ic_button_text' => 'Learn More',
				'ic_button_link' => '#',
				'ic_css_classes' => '',
				'form_header' => 'Default Header',
				'form_icon_one_url' => '',
				'form_icon_one_css' => '',
				'form_icon_one_text' => 'Text One',
				'form_icon_two_url' => '',
				'form_icon_two_css' => '',
				'form_icon_two_text' => 'Text Two',
				'form_icon_three_url' => '',
				'form_icon_three_css' => '',
				'form_icon_three_text' => 'Text Three',
				'form_field_place_one' => 'Name',
				'form_field_place_two' => 'Email',
				'form_field_place_three' => 'Goals',
				'form_button_text' => 'Learn More',
				'form_button_link' => '#',
				'blockquote_icon_url' => '',
				'blockquote_css_classes' => '',
				'blockquote_background_image_url' => '',
				'blockquote_quote' => '',
				'blockquote_button_text' => 'Button Text',
				'blockquote_button_link' => '#',
				'simple_image_url' => ''
			);

			if ($count) {
				$this->create_column($column, $count);
			} else {
				die(-1);
			}

			die();
    }

  	/**
	 	 * Sanitize widget form values as they are saved.
	 	 *
	 	 * @see WP_Widget::update()
	 	 *
	 	 * @param array $new_instance Values just sent to be saved.
	 	 * @param array $old_instance Previously saved values from database.
	 	 *
	   * @return array Updated safe values to be saved.
	 	 */
  	public function update( $new_instance, $old_instance ) {
  		error_log("Old instance:");
  		error_log(print_r($old_instance, true));
  		error_log("New instance:");
  		error_log(print_r($new_instance, true));
  		return $new_instance;
  	}
}	// class paramount_sections

?>