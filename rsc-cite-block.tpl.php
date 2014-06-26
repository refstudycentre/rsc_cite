<?php
  $type = (empty($node->type) ? "" : $node->type);
  $author = l($author->name,"taxonomy/term/{$author->tid}");
  $citation = render($variables['citation']);
  
  print "<$tag class='$type citation'>$citation</$tag>";
  print "<$tag class='$type author'>$author</$tag>";
?>
