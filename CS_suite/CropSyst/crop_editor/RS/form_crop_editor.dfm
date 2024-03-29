object crop_editor_form: Tcrop_editor_form
  Left = 0
  Top = 0
  Caption = 'Crop:'
  ClientHeight = 659
  ClientWidth = 702
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001002020100000000000E80200001600000028000000200000004000
    0000010004000000000080020000000000000000000000000000000000000000
    0000000080000080000000808000800000008000800080800000C0C0C0008080
    80000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00002A
    222CCCCCCC2A2CCCCCCCCCCB3CCC0222A2A2CCCA222AAC22ACCCCCCB3CCCA2A2
    22AA2CCC2ACA2A2CCCCCCCCBBCCCA22A22222CCCCCCA2CCCCCCCCCB3BCCCA222
    2A222C22CC2A22C22CCCCCBBBCCCA22A22AA2CCAC2AA2A22CCCCCC3B3CCCA22A
    22222CCC2ACA2CACCCCCCCBBCBCCCA22AA22CCCCCCCA2CCCCCCCCCB3CBCCCCA2
    222CCC2CCC2A2CCC2CCCCBCBC3CCCCCAAACCCC22C2AA22C22CCCC3CBCBCCCCCC
    CCCCCCCA2ACA2A22CCCCCBCBCCCCCCCCCCCCCCCCCCCA2CACCCCCCCCBCCCCCCCC
    CCCCCCCCCCCA2CCCCCCCCCCBCCCCCCCCCCCCCCCCCCCA2C22CCCCCCBBBCCCCCCC
    CCCCCCCA2C2A222CCCCCCCBBBCCCCCCCCCCCCCCC22A22ACCCCCCCCBBBCCCCCCC
    CCCCCCCCA2CACCCCCCCCCCCCCCCCCCCCCCCCCCC2CCC2CCC2CCCCCCCCCCCCCCCC
    CCCCCCCACC2ACCC2CCCCCCCCCCCCCCCCCCCCCCCC22C22C2CCCCCCCCCCCCCCCCC
    CCCCCCCCACCAA2ACCCCCCCCCCCCCCCCCCCCCCCCCCC22CCCCCCCCCCCCCCCCCCCC
    CCCCCCC2C2CACCCCCCCCCCCCCCCCCCCCCCCCCCCA2AC22CCCCCCCCCCCCCCCCCCC
    CCCCCCCCCCCAA22CCCCCCCCCCCCCCCCCCCCCCCCCCCC2CACCCCCCCCCCCCCCCCCC
    CCCCCCCCCCCBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCBCCCCCCCCCCCCCCCCCCCC
    CCCCCCCCCCCBCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
    CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC000
    0000800000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000000000000000000000000000}
  OldCreateOrder = False
  OnActivate = FormActivate
  OnKeyPress = FormKeyPress
  PixelsPerInch = 96
  TextHeight = 13
  object scrollbox_parameters: TScrollBox
    Left = 177
    Top = 0
    Width = 525
    Height = 659
    VertScrollBar.Position = 1822
    Align = alClient
    BevelInner = bvNone
    BevelOuter = bvNone
    BorderStyle = bsNone
    TabOrder = 0
    object groupbox_classification: TGroupBox
      Left = 0
      Top = -1822
      Width = 508
      Height = 169
      Align = alTop
      Caption = 'Classification'
      TabOrder = 0
      object note_harvested_biomass: TLabel
        Left = 146
        Top = 87
        Width = 171
        Height = 42
        AutoSize = False
        Caption = 'Make sure to select the harvested biomass (not Unknown).'
        Color = clRed
        ParentColor = False
        WordWrap = True
      end
      object radiogroup_landuse: TRNAutoRadioGroup
        Left = 323
        Top = 6
        Width = 140
        Height = 81
        Caption = 'Land use'
        Items.Strings = (
          'Fallow or untilled rows'
          'Row crops'
          'Small grain'
          'Closed seed legumes'
          'Pasture')
        TabOrder = 0
      end
      object radiogroup_life_cycle: TRNAutoRadioGroup
        Left = 3
        Top = 16
        Width = 137
        Height = 71
        Caption = 'Life cycle'
        Items.Strings = (
          'Indeterminate'
          'Annual'
          'Biennial'
          'Perennial')
        TabOrder = 1
        OnClick = radiogroup_life_cycleClick
      end
      object radiogroup_photosynthetic: TRNAutoRadioGroup
        Left = 3
        Top = 87
        Width = 137
        Height = 58
        Caption = 'Photosynthetic pathway'
        Items.Strings = (
          'C3'
          'C4'
          'CAM')
        TabOrder = 2
      end
      object radiogroup_harvested_biomass: TRNAutoRadioGroup
        Left = 146
        Top = 6
        Width = 171
        Height = 81
        Caption = 'Harvested biomass'
        Columns = 2
        Items.Strings = (
          'Seed/ackene'
          'Root'
          'Tuber'
          'Leaf'
          'Fruit'
          'Canopy'
          'Everything'
          'Unknown')
        TabOrder = 3
        OnClick = radiogroup_harvested_biomassClick
      end
      object checkbox_grape: TRNAutoCheckBox
        Left = 154
        Top = 123
        Width = 55
        Height = 17
        Caption = 'Grape'
        State = cbGrayed
        TabOrder = 4
        OnClick = checkbox_grapeClick
      end
      object checkbox_tea: TRNAutoCheckBox
        Left = 240
        Top = 123
        Width = 53
        Height = 17
        Caption = 'Tea'
        State = cbGrayed
        TabOrder = 5
      end
      object radiogroup_stem_type: TRNAutoRadioGroup
        Left = 323
        Top = 87
        Width = 136
        Height = 58
        Caption = 'Stem type'
        Items.Strings = (
          'Herbaceous'
          'Woody')
        TabOrder = 6
      end
      object combobox_RUSLE2_description: TRNAutoComboBox
        Left = 2
        Top = 146
        Width = 504
        Height = 21
        Hint = 
          'This can be used to set the crop description.  Some CS suite mod' +
          'ules require the description to be one of these RUSLE2 crop desc' +
          'riptions'
        Align = alBottom
        ParentShowHint = False
        ShowHint = True
        TabOrder = 7
        OnChange = combobox_RUSLE2_descriptionChange
        Items.Strings = (
          'Alfalfa brome, fall seed senes to yr2 regrowth '
          'Alfalfa brome, spring seed '
          'Alfalfa brome, spring seed regrowth after cut '
          'Alfalfa brome, spring seed senes to y2 regrowth '
          'Alfalfa brome, spring seed senes winter graze to y2 regrowth '
          'Alfalfa brome, yr2 regrowth after cut'
          'Alfalfa brome, yr2 senes to yr3 regrowth'
          'Alfalfa brome, yr2 senes winter graze to yr3 regrowth'
          'Alfalfa brome, yr3 regrowth  after cut'
          'Alfalfa brome, yr3 senes to yr4 regrowth'
          'Alfalfa brome, yr3 senes winter graze to yr4 regrowth'
          'Alfalfa brome_oat silage, harvtoyr2_1stcut'
          'Alfalfa brome_oat silage, spring seed '
          'Alfalfa brome_oat,  seed '
          'Alfalfa brome_oat, harv to yr2  1st cut'
          'Alfalfa seed nw, fall plant senes to yr2 clipback'
          'Alfalfa seed nw, spring plant'
          'Alfalfa seed nw, yr2 regrowth after clipback'
          'Alfalfa seed nw, yr2 senes to yr3 regrowth'
          'Alfalfa seed sw, fall plant senes to yr2 clipback'
          'Alfalfa seed sw, yr2 regrowth after clipback'
          'Alfalfa seed sw, yr2 senes to yr3 regrowth'
          'Alfalfa, fall seed senes to yr2 regrowth '
          'Alfalfa, NW fall seeding, senes to yr2 regrowth'
          'Alfalfa, NW spring seeding, senes to yr2 regrowth'
          'Alfalfa, NW yr2 regrowth after cutting '
          'Alfalfa, NW yr2 senes to yr3 regrowth'
          'Alfalfa, NW yr3 regrowth after cutting '
          'Alfalfa, NW yr3 senes to yr4 regrowth'
          'Alfalfa, South, fall seed senes to yr2 regrowth'
          'Alfalfa, South, spring seed senes to y2 regrowth'
          'Alfalfa, South, yr2 regrowth after cutting'
          'Alfalfa, South, yr2 senes to yr3 regrowth'
          'Alfalfa, South, yr3 regrowth after cutting'
          'Alfalfa, South, yr3 senes to yr4 regrowth'
          'Alfalfa, South, yr4 regrowth after cutting'
          'Alfalfa, South, yr4 senes to yr5 regrowth'
          'Alfalfa, spring seed '
          'Alfalfa, spring seed regrowth after cutting '
          'Alfalfa, spring seed senes to y2 regrowth'
          'Alfalfa, spring seed senes winter graze to y2 regrowth'
          'Alfalfa, yr2 regrowth after cutting '
          'Alfalfa, yr2 senes to yr3 regrowth'
          'Alfalfa, yr2 senes winter graze to yr3 regrowth'
          'Alfalfa, yr3 regrowth after cutting '
          'Alfalfa, yr3 regrowth after seed harvest '
          'Alfalfa, yr3 senes to yr4 regrowth'
          'Alfalfa, yr3 senes winter graze to yr4 regrowth'
          'Alfalfa, yr4 regrowth after cutting '
          'Alfalfa, yr4 senes to yr5 regrowth'
          'Alfalfa, yr4 senes winter graze to yr5 regrowth'
          'Alfalfa_oat(silage), spring seed'
          'Alfalfa_oat, grwth after oat harv to y2 regrth'
          'Alfalfa_oat, nurse crop spring seed'
          'Alfalfa_oat, silage harv to yr2  1st cut'
          'Almond, Pistachio; bare ground'
          'Almond, Pistachio; fall sm grain cov crop'
          'Almond, Pistachio; permanent grass cov crop'
          'Almond, Pistachio; weeds annual cover'
          'Amaranth'
          'Artichoke, annual'
          'Artichoke, perennial'
          'Asparagus, yr1 and 2'
          'Asparagus, yr1 cover crop'
          'Asparagus, yr2 cover crop'
          'Asparagus, yr3+  HI PROD'
          'Asparagus, yr3+ cover crop'
          'Asparagus, yr3+'
          'Bahiagrass, y2+ senesc thru  yr3 + greenup'
          'Bahiagrass, yr1 spring seeded'
          'Bahiagrass, yr2+ regrowth'
          'Barley, annual winter vineyard cover, CA, 3 mowings'
          'Barley, annual winter vineyard cover, CA'
          'Barley, spring, CMZ 10, 10 in. spac.'
          'Barley, spring, CMZ 47, 7 in. spac.'
          'Barley, spring, CMZ 50, 7 in. spac.'
          'Barley, spring, hay'
          'Barley, spring'
          'Barley, winter, CMZ 10, 10 in. spac.'
          'Barley, winter, CMZ 47, 7 in. spac.'
          'Barley, winter, hay'
          'Barley, winter'
          'Bean, field 30in rows'
          'Bean, field 7in rows'
          'Bean, green snap hand pick'
          'Bean, green snap mech harv HI PROD'
          'Bean, green snap mech harv'
          'Bean, lima'
          'Bean, pinto'
          'Beans, garbonzo'
          'Beet, red garden var'
          'Belladonna, yr1'
          'Belladonna, yr2'
          'Bermudagrass, 1st yr'
          'Bermudagrass, coastal regrowth yr2+'
          'Bermudagrass, coastal spring seeded 1st year'
          'Bermudagrass, coastal, y2+ senesc thru  yr3 + greenup'
          'Bermudagrass, common regrowth yr2+'
          'Bermudagrass, common spring seeded 1st year'
          'Bermudagrass, common, y1 regrowth and senesc thru  yr2 greenup'
          'Bermudagrass, common, y2+ senesc thru  yr3 + greenup'
          'Blueberry, low bush, cov crop'
          'Blueberry, low bush'
          'Blueberry, med bush'
          'Blueberry, tall bush'
          'Bluestem, old world, established,  regrowth after grazing or hay'
          'Bluestem, old world, established, senesc thru spring regrowth'
          'Brassica pasture, forage turnip'
          'Brocolli and Cabbage seed'
          'Brocolli, fall'
          'Brocolli, HI PROD'
          'Brocolli, spring'
          'Brome, annual (Blando), winter vineyard cover, CA'
          'Brome, California, established vineyard cover, CA'
          'Brome, California, seeding period, vineyard cover, CA'
          'Bromegrass, fall seed senes to yr2 regrowth '
          
            'Bromegrass, hay, yr2 overwinter to yr3 regrowth after fall after' +
            'math removal'
          'Bromegrass, hay, yr2 regrowth after cut'
          'Bromegrass, hay, yr2 senes to yr3 regrowth'
          
            'Bromegrass, hay, yr3 plus, overwinter to yr4 plus, regrowth afte' +
            'r fall aftermath removal'
          'Bromegrass, hay, yr3 regrowth after cut '
          'Bromegrass, hay, yr3 senes to yr4 regrowth'
          'Bromegrass, spring seed 1st yr'
          'Buckwheat  '
          'Cabbage, for kraut'
          'Cabbage, HI PROD'
          'Cabbage'
          'Canola, spring '
          'Canola, winter '
          'Cantaloupe, HI PROD'
          'Cantaloupe'
          'Carrot and Onion seed'
          'Carrot, fresh market'
          'Carrot, hybrid seed production'
          'Carrot, processing after small grain cover'
          'Carrot, processing'
          'Cauliflower, HI PROD'
          'Cauliflower'
          'Celery, HI PROD'
          'Celery'
          'Chickpea '
          'Chicory; seed Yr 1'
          'Chicory; seed Yr 2'
          'Citrus, bare ground'
          'Citrus, cover in alley'
          'Citrus, full cover'
          'Clover white seed; estb regrowth to seed harv.'
          'Clover white seed; fall plant sens. to 2yr. hay or graze'
          'Clover white seed; fall planted'
          'Clover, annual, seed'
          'Clover, annual'
          'Clover, red  spring seed'
          'Clover, red late summer seed senes to yr2 regrowth'
          'Clover, red y1 regrowth after cut '
          'Clover, red y1 regrowth after small grain havest'
          'Clover, red y1 senesc to y2 regrow'
          'Clover, red y2 regrow after cut'
          'Clover, red, NW spr seed, harv seed'
          'Clover, red, NW spr seed, senes to yr2 regrowth'
          'Clover, red, NW yr2 regrowth to seed harv'
          'Clover, red, y2 senesc to yr3 regrowth'
          'Clover, red, y3 regrow after cut'
          'Clover, red, y3 senesc to spring greenup'
          'Clover, red_oat nurse crop spring seed'
          'Clover, red_timothy, fall seed senes to yr2 regrowth'
          'Clover, red_timothy, spring seed regrowth after cut'
          'Clover, red_timothy, spring seed senes to y2 regrowth'
          'Clover, red_timothy, spring seed'
          'Clover, red_timothy, y2 regrow after cut'
          'Clover, red_timothy, y2 senesc to yr3 regrowth'
          'Clover, red_timothy_oat nurse crop spring seed'
          'Clover, sweet, for seed or green manure'
          'Clover, sweet, spring seed '
          'Clover, sweet, spring seed regrowth after cutting '
          'Clover, sweet, spring seed senes to y2 regrowth'
          'Clover, sweet, yr2 regrowth after cutting '
          'Clover, sweet, yr2 senescence and death '
          'Collard greens, HI PROD'
          'Collard, greens'
          'Corn, grain  '
          'Corn, grain  18 in rows'
          'Corn, grain with grass weeds'
          'Corn, grain, 38 in rows'
          'Corn, grain'
          'Corn, pop'
          'Corn, seed'
          'Corn, silage  18 in rows '
          'Corn, silage '
          'Corn, silage 18 in. rows with interseed Rye cover '
          'Corn, silage and grass cover'
          'Corn, silage and legume cover'
          'Corn, silage with grass weeds'
          'Corn, silage with interseed Rye cover '
          'Corn, sweet '
          'Corn, sweet HI PROD'
          'Corn, with interseed Rye cover '
          'Corn, with Squash and Beans'
          'Cotton, CA Hi Prod.'
          'Cotton, delta'
          'Cotton, southern upland'
          'Cotton, stripper skip 1 row'
          'Cotton, stripper skip 2 rows'
          'Cotton, stripper'
          'Cotton, ultra narrow row'
          'Cow pea'
          'Crambe'
          'Cranberries, 3 year establishment'
          'Cranberries, established stand'
          'Cucumber, HI PROD'
          'Cucumber'
          'Delayed germination'
          'Dichondra, establishment'
          'Dichondra, overwinter and spring regrowth'
          'Eggplant, HI PROD'
          'Eggplant'
          'Fescue, annual (Zorro), winter vineyard cover, CA'
          'Fescue, red, established vineyard cover, CA'
          'Fescue, red, seeding period, vineyard cover, CA'
          'Filbert, full cover'
          'Filbert, no cover'
          'Flax (lbs)'
          'Flax '
          'Flowers, seed'
          'Flowers'
          'Garlic seed production'
          'Ginseng, lath cover, 3 yr stand'
          'Ginseng, lath cover, 4 yr stand'
          'Ginseng, mesh cover, 3 yr stand'
          'Ginseng, mesh cover, 4 yr stand'
          'Gladiola'
          'Grama, yr 1'
          'Grama, yr 2'
          'Grama, yr 3+'
          'Grass seed, established, senescence'
          'Grass seed, fall seeding'
          'Grass seed, interseeded growing crop'
          'Grass seed, per. ryegrass fall plant late'
          'Grass seed, per. ryegrass late plant, established, senescence'
          'Grass seed, spring seeded'
          'Grass seed, y2 and later regrowth to seed harvest'
          'Grass seed, yr2 senescence'
          'Grass, cool season pasture, regrowth after grazing'
          'Grass, cool season pasture, senesc to regrowth'
          'Grass, cool season pasture, slow regrowth after grazing'
          'Grass, cool season, fall seed senes to yr2 regrowth SE'
          'Grass, cool season, fall seeded '
          'Grass, cool season, spring seeded '
          'Grass, cool season, y2+ senesc thru  yr3 + greenup SE'
          'Grass, dense,  senesc to 1st cut'
          'Grass, dense, regrowth after cut'
          'Grass, warm season pasture yr2, regrowth after grazing'
          'Grass, warm season pasture, spring seeded '
          'Grass, warm season pasture, y2 senesc to yr3 regrowth'
          'Grass, warm season pasture, yr3+ , senesc to regrowth'
          'Grass, warm season pasture, yr3+,  regrowth after grazing'
          'Grass, warm season pasture, yr3+,  slow regrowth after grazing'
          'Greens, baby leaf HI PROD'
          'Guar'
          'Hardwood, established stands'
          'Hardwood, planted stands'
          'Horseradish'
          'Kale, HI PROD'
          'Kale'
          'Kenaf'
          'Lavender, established stand'
          'Lentils, fall'
          'Lentils, grazed out'
          'Lentils'
          'Lettuce, head HI PROD'
          'Lettuce, head'
          'Lettuce, leaf HI PROD'
          'Lettuce, leaf'
          'Bulbs, lilly'
          'Lily, Cala yr1'
          'Lily, Cala yr2'
          'Lily, Cala yr3'
          'Lily, Cala yr4+'
          'Millet, proso '
          'Mint, fall seed'
          'Mint, spring seed '
          'Mint, spring seed regrowth after cutting '
          'Mint, spring seed senes to y2 regrowth '
          'Mint, yr2 regrowth after cutting '
          'Mint, yr2 senes to yr3 regrowth '
          'Mint, yr3 regrowth after cutting '
          'Mint, yr3 senes to yr4 regrowth '
          'Mint, yr4 regrowth after cutting '
          'Mint, yr4 senes to yr5 regrowth '
          'Mungbean'
          'Muskmelon'
          'Mustard greens, fall'
          'Mustard greens, HI PROD'
          'Mustard greens, spring'
          'Mustard seed, spring '
          'Oat forage, fall seed to 1st cut or graze, deep south'
          'Oat forage, regrowth, deep south'
          'Oats, fall seeded cover '
          'Oats, fall seeded, hay '
          'Oats, hay regrowth after cut'
          'Oats, spring '
          'Oats, spring hay'
          'Oats, spring silage_hay '
          'Oats, spring silage'
          'Oats, winter S.E.'
          'Okra, HI PROD'
          'Okra'
          'Onion seed production'
          'Onion, after sm grain'
          'Onion, bulb dry HI PROD'
          'Onion, bulb dry'
          'Onion, dry bulb, Vidalia-type'
          'Onion, green bunch'
          'Orchard, bare ground'
          'Orchard, cover betw rows 50_50'
          'Orchard, cover betw rows 80_20'
          'Orchard, cover betw rows 90_10'
          'Orchard, cover betw rows'
          'Orchard, cover betw rows70_30'
          'Orchard, dwarf varieties'
          'Orchard, fall small grain cover crop'
          'Orchard, full cover'
          'Orchard, new yr1'
          'Orchard, new yr2'
          'Orchard, new yr3'
          'Orchard, new yr4'
          'Orchard, new yr5'
          'Orchard, weeds annual cover'
          'Orchardgrass, fall seed senes to yr2 regrowth '
          'Orchardgrass, seeding '
          'Orchardgrass, y2 regrowth after cut'
          'Orchardgrass, y2 senesc to 1st cut yr3'
          'Orchardgrass, y3 regrowth after cut'
          'Orchardgrass, y3 senesc to 1st cut yr4'
          'Peanut, runner, twin-row'
          'Peanut, runner'
          'Peas, Austrian winter, northern climate'
          'Peas, Austrian winter'
          'Peas, field 7in rows'
          'Peas, green HI PROD'
          'Peas, green, drilled'
          'Peas, green, rows'
          'Peas, southern'
          'Peas, spring, grazed out'
          'Peas, spring, hay'
          'Peas, spring, NWRR  '
          'Pecan, Walnut; bare ground'
          'Pecan, Walnut; fall sm. grain cover crop'
          'Pecan, Walnut; perm. grass cover crop'
          'Pecan, Walnut; weeds annual cover'
          'Peppers, bell HI PROD'
          'Peppers, bell'
          'Peppers, chili HI PROD'
          'Peppers, hot'
          'Pine bluegrass, seeding period, vineyard cover, CA'
          'Pine blugrass,  established perennial vineyard cover, CA'
          'Pine, longleaf planted stands'
          'Pine, planted stands'
          'Potato, high yield'
          'Potato, Irish'
          'Potato, seed production'
          'Potato, sweet'
          'Pumpkin'
          'Radish, HI PROD'
          'Radish, oilseed'
          'Radish'
          'Range Desert grassland'
          'Range Northern desert shrub'
          'Range Northern mixed grass prairie seeding'
          'Range Northern mixed grass prairie'
          'Range Short grass prairie'
          'Range Southern desert shrub'
          'Range Southern mixed grass prairie seeding'
          'Range Southern mixed grass prairie'
          'Range Tall grass prairie'
          'Raspberry, black '
          'Raspberry, black, cov crop'
          'Raspberry, black, perm cov'
          'Rice, AR, flooded  '
          'Rice, CA, flooded, semi-dwarf varieties'
          'Rice, CA, flooded, tall varieties'
          'Rice, CA, winter flooded , semi-dwarf varieties'
          'Rice, CA, winter flooded, tall varieties'
          'Rice, dwarf var. CA, flooded'
          'Rice, dwarf var. CA, winter flooded'
          'Rice, dwarf var. CA'
          'Rice, ratoon crop, AR, flooded'
          'Rice, wild'
          'Rye and Hairy vetch, winter cover '
          'Rye, cereal '
          'Rye, cereal forage, fall seed to 1st cut or graze, deep south'
          'Rye, cereal forage, regrowth, deep south'
          'Rye, cereal interseeded growing cover'
          'Rye, cereal silage'
          'Rye, spring 7in rows'
          'Rye, winter cover '
          'Rye, winter cover S.E.'
          'Rye, winter grazed S.E.'
          'Rye, winter, grain S.E.'
          'Rye, winter, silage, SE'
          'Ryegrass, ann, seed prod, regrowth'
          'Ryegrass, ann, seed prod'
          'Ryegrass, annual, fall interseeded in established bermudagrass'
          'Ryegrass, annual, fall seeding, deep south'
          'Ryegrass, annual, regrowth after grazing or hay, deep south'
          'Ryegrass, annual, regrowth'
          'Ryegrass, annual, seeding yr'
          'Ryegrass, peren. interseeded growing cover'
          'Safflower'
          'Sericean Lespedeza, hay'
          'Sesame, seed'
          'Sesame'
          'Silvopasture'
          'Small grain, spring, hay'
          'Small grain, winter, hay'
          'Sorghum, forage fall regrowth'
          'Sorghum, forage regrowth'
          'Sorghum, forage seeding to 1st cut'
          'Sorghum, forage, grazed'
          'Sorghum, grain '
          'Sorghum, grain 18 inch rows'
          'Sorghum, grain South 135day'
          'Sorghum, grain, hi biomass var.'
          'Sorghum, grain, South, shrt seasn hi density'
          'Sorghum, silage '
          'Sorghum, sudangrass, 7in rows regrowth after cut'
          'Sorghum, sudangrass, 7in rows, seeding to 1st cut'
          'Soybean, 15 in with interseeded rye cover'
          'Soybean, 30 in with interseeded rye cover'
          'Soybean, group 0 and I,  30in rows'
          'Soybean, group 0 and I,  7in rows'
          'Soybean, group II, III and IV 30 in rows'
          'Soybean, group II, III and IV,  7in rows'
          'Soybean, groups V, VI, VII, and VIII 30 in rows'
          'Soybean, groups V, VI, VII, and VIII 7in rows'
          'Soybean, millet hay '
          'Soybean, mw 15 - 20 in rows'
          'Soybean, mw 30 in rows, relay cropped'
          'Soybean, mw 30 in rows'
          'Soybean, mw 7in rows'
          'Soybean, southern 15-20 in rows'
          'Soybean, southern 30 in rows'
          'Soybean, southern 7in rows '
          'Spinach, HI PROD'
          'Spinach'
          'Squash, HI PROD'
          'Squash'
          'Strawberry, annual,1st pick Ca and FL'
          'Strawberry, annual,2nd and later pick Ca and FL'
          'Strawberry, establishment  yr'
          'Strawberry, high yield , yr2+'
          'Strawberry, high yield, yr1'
          'Strawberry, yr2+'
          'Sugarbeet, seed'
          'Sugarbeet, sugar'
          'Sugarbush, established stands'
          'Sugarcane establishment to 1st harvest'
          'Sugarcane, 1st ratoon'
          'Sugarcane, 2nd ratoon'
          'Sugarcane, 3rd ratoon'
          'Sugarcane, 4th ratoon'
          'Sugarcane, plus weeds, 1st ratoon'
          'Sugarcane, plus weeds, 2nd ratoon'
          'Sugarcane, plus weeds, 3rd ratoon'
          'Sugarcane, plus weeds, 4th ratoon'
          'Sugarcane, plus weeds, planting to 1st harvest '
          'Sunflower'
          'Switchgrass bimass prod, spring seeded '
          'Switchgrass biomass prod yr1 overwinter, and yr2 regrowth '
          'Switchgrass, biomass prod. yr2 overwinter and yr3   '
          'Tall Fescue pasture,  senesc to regrowth'
          'Tall fescue pasture, regrowth after grazing SE'
          'Tall fescue pasture, regrowth after grazing'
          'Tall fescue pasture, slow regrowth after grazing'
          'Tall Fescue pasture, summer dormancy to fall regrowth South'
          'Tall fescue y3+, regrowth after cut'
          'Tall Fescue, fall seed senes to yr2 regrowth '
          'Tall Fescue, fall seed senes to yr2 regrowth SE'
          'Tall fescue, release growth after nurse crop harvest'
          'Tall fescue, spring seed 1st year'
          'Tall fescue, y2 regrowth after cut SE'
          'Tall fescue, y2 regrowth after cut'
          'Tall fescue, y2 senesc thru  yr3 greenup'
          'Tall fescue, y2+ senesc thru  yr3 + greenup SE'
          'Tall fescue, y3 regrowth after cut SE'
          'Tall Fescue_Wheat, winter, grain S.E.'
          'Timothy, fall seed senes to yr2 regrowth '
          'Timothy, release growth after nurse crop harvest'
          'Timothy, spring seed senesc to yr2 regrowth'
          'Timothy, yr2 regrowth after cut'
          'Timothy, yr2 senesc to yr3 1st cut'
          'Timothy, yr3 regrowth after cuts'
          'Timothy, yr3 senesc to yr4 regrowth'
          'Tobacco, burley'
          'Tobacco, dark'
          'Tobacco, flue cured'
          'Tomatilla, staked'
          'Tomato, fresh mkt staked'
          'Tomato, fresh mkt'
          'Tomato, processing'
          'Tree nurs, bare rt, fall pl, cov yr1'
          'Tree nurs, bare rt, fall pl, cov yr2'
          'Tree nurs, bare rt, fall pl, cov yr3'
          'Tree nurs, bare rt, fall pl, yr1'
          'Tree nurs, bare rt, fall pl, yr2'
          'Tree nurs, bare rt, fall pl, yr3'
          'Tree nurs, bare rt, fall pl, yr4'
          'Tree nurs, bare rt, spr pl, cov, yr1'
          'Tree nurs, bare rt, spr pl, cov, yr2'
          'Tree nurs, bare rt, spr pl, yr1'
          'Tree nurs, bare rt, spr pl, yr2'
          'Tree nurs, bare rt, spr pl, yr3'
          'Tree, nursery clean till'
          'Trees, Christmas grass strip, established stand, selective cut'
          'Trees, Christmas grass strip'
          'Trees, Christmas yr1'
          'Trees, Christmas yr2'
          'Trees, Christmas yr3'
          'Trees, Christmas yr4'
          'Trees, Christmas yr5'
          'Trees, Christmas yr6'
          'Trees, Christmas yr7'
          'Trees, Christmas yr8'
          'Triticale, silage'
          'Triticale'
          'Turfgrass established'
          'Turfgrass, bermuda spring seed'
          'Turfgrass, fall seed'
          'Turfgrass, spring seed'
          'Turnip, fall'
          'Turnip, spring'
          'Vetch, hairy, deep south'
          'Vetch, hairy'
          'Vineyard, clean till yr 1'
          'Vineyard, clean till yr 2+'
          'Vineyard, cover betw rows 9ft spac'
          'Vineyard, cover crop'
          'Vineyard, cover in alley'
          'Vineyard, full cover'
          'Vineyard, yr1 cover'
          'Vineyard, yr1'
          'Watermelon, HI PROD'
          'Watermelon'
          'Weeds, 12+ months'
          'Weeds, exposed after harvest less than 3 mo growth'
          'Weeds, less than 3 mo growth'
          'Weeds, less than 6 mo growth'
          'Weeds, less than 9 mo growth'
          'Weeds, winter, Central'
          'Weeds, winter, Deep SO'
          'Weeds, winter, Mid South'
          'Wheat, regrowth after winter grazing'
          'Wheat, spring 14in rows'
          'Wheat, spring 7in rows'
          'Wheat, spring graze grain'
          'Wheat, spring graze out'
          'Wheat, spring, CMZ 10, 10 in. spac.'
          'Wheat, spring, CMZ 47, 7 in. spac.'
          'Wheat, spring, CMZ 50, 7-10 in. spac.'
          'Wheat, spring, hay, 7in rows'
          'Wheat, winter 10in 2-1 skip row'
          'Wheat, winter 14in rows'
          'Wheat, winter 7in rows'
          'Wheat, winter cover'
          'Wheat, winter graze out South'
          'Wheat, winter graze, grain'
          'Wheat, winter grazed forage'
          'Wheat, winter nurse crop  w_ red clover '
          'Wheat, winter silage S.E.'
          'Wheat, winter silage'
          'Wheat, winter south 7in rows'
          'Wheat, winter, 10in rows and relay intercropped soybeans'
          'Wheat, winter, CMZ 10, 10 in. spac. delayed germ.'
          'Wheat, winter, CMZ 10, 10 in. spac. early plant'
          'Wheat, winter, CMZ 10, 10 in. spac. late plant'
          'Wheat, winter, CMZ 47, 7-10 in. spac. late plant'
          'Wheat, winter, CMZ 47, 7-10 in.spac. early plant'
          'Wheat, winter, CMZ 50 hi ppt, 14-16 in. spac. early plant'
          'Wheat, winter, CMZ 50 hi ppt, 14-16 in. spac. late plant old'
          'Wheat, winter, CMZ 50 hi ppt, 14-16 in. spac. late plant'
          'Wheat, winter, CMZ 50 hi ppt, 7-10 in. spac. early plant'
          'Wheat, winter, CMZ 50 hi ppt, 7-10 in. spac. late plant'
          'Wheat, winter, CMZ 50 lo ppt, 16-18 in. spac. early plant'
          'Wheat, winter, CMZ 50 lo ppt, 16-18 in.spac. late plant'
          'Wheat, winter, cover S.E.'
          'Wheat, winter, grain and timothy'
          'Wheat, winter, grain S.E.'
          'Wheat, winter, grain with timothy'
          'Wheat, winter, hay or silage, CMZ 10, 10 in. spac.'
          'Wheat, winter, hay, 7in rows'
          'Wheat, winter, mid-south'
          'Wheat, winter, silage, mid south, 7in rows'
          'Wheat, winter, west coast'
          
            'Wheat, winter, with winter peas, CMZ 50 hi ppt, 14-16 in. spac. ' +
            'late plant'
          
            'Wheat, winter, with winter peas, CMZ 50 hi ppt, 7-10 in. spac. l' +
            'ate plant'
          
            'Wheat, winter, with winter peas, CMZ 50 lo ppt, 16-18 in.spac. l' +
            'ate plant'
          'Yarrow')
      end
    end
    object groupbox_layout: TGroupBox
      Left = 0
      Top = 401
      Width = 508
      Height = 200
      Align = alTop
      Caption = 'Layout'
      TabOrder = 1
      object groupbox_orientation: TGroupBox
        Left = 2
        Top = 15
        Width = 504
        Height = 82
        Align = alTop
        Caption = 'Orientation'
        TabOrder = 0
        object edit_tree_spacing: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 57
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Spacing within rows'
          TabOrder = 0
        end
        object edit_row_spacing: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 36
          Width = 494
          Height = 21
          Hint = '0 = North, 90 = East, 180 = South,  270 = West'
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Spacing between rows'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
        end
        object edit_row_azimuth: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 15
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Row azimuth'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 2
        end
      end
      object groupbox_canopy_characteristics: TGroupBox
        Left = 2
        Top = 97
        Width = 504
        Height = 104
        Align = alTop
        Caption = 'Canopy characteristics'
        TabOrder = 1
        object Label5: TLabel
          Left = 238
          Top = 0
          Width = 26
          Height = 13
          Caption = 'Initial'
        end
        object Label8: TLabel
          Left = 328
          Top = 0
          Width = 22
          Height = 13
          Caption = 'Final'
        end
        object edit_final_row_height: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 72
          Width = 494
          Height = 21
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Tree height'
          TabOrder = 0
          object edit_initial_row_height: TRNAutoFloatEdit
            Left = 322
            Top = 0
            Width = 70
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alRight
            Alignment = taRightJustify
            Lines.Strings = (
              '0')
            TabOrder = 5
          end
        end
        object edit_final_tree_breadth: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 45
          Width = 494
          Height = 21
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Tree width along the row '
          TabOrder = 1
          object edit_initial_tree_breadth: TRNAutoFloatEdit
            Left = 322
            Top = 0
            Width = 70
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alRight
            Alignment = taRightJustify
            Lines.Strings = (
              '0')
            TabOrder = 5
          end
        end
        object edit_final_row_width: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 18
          Width = 494
          Height = 21
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Tree width across row'
          TabOrder = 2
          object edit_initial_row_width: TRNAutoFloatEdit
            Left = 322
            Top = 0
            Width = 70
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alRight
            Alignment = taRightJustify
            Lines.Strings = (
              '0')
            TabOrder = 5
          end
        end
        object edit_branch_insertion_height: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 96
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Branch insertion height'
          TabOrder = 3
        end
      end
    end
    object groupbox_hydrothermal_time: TGroupBox
      Left = 0
      Top = 635
      Width = 508
      Height = 146
      Align = alTop
      Caption = 'Hydrothermal time'
      TabOrder = 2
      object edit_b: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 120
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'b intercept for hydrothermal time constant curve'
        TabOrder = 0
      end
      object edit_a: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 99
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'a intercept for hydrothermal time constant curve'
        TabOrder = 1
      end
      object edit_stdev_base_soil_water_pot: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 78
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'StdDev base soil water potential'
        TabOrder = 2
      end
      object edit_mean_base_soil_water_pot: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 57
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Mean base soil water potential for 50% emergence'
        TabOrder = 3
      end
      object edit_germination_base_temperature: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 36
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Germination base temperature'
        TabOrder = 4
      end
      object edit_seedling_leaf_area: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 15
        Width = 498
        Height = 21
        Hint = 'on first day of emergence'
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Seedling leaf area'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 5
      end
    end
    object groupbox_thermal_time: TGroupBox
      Left = 0
      Top = -1653
      Width = 508
      Height = 104
      Margins.Top = 0
      Margins.Bottom = 0
      Align = alTop
      Caption = 'Thermal time accumulation'
      TabOrder = 3
      object radiogroup_resolution: TRNAutoRadioGroup
        Left = 2
        Top = 15
        Width = 68
        Height = 87
        Align = alLeft
        Caption = 'Resolution'
        Items.Strings = (
          'daily'
          'hourly')
        TabOrder = 0
        OnClick = radiogroup_resolutionClick
      end
      object panel_GGD_ranges: TPanel
        Left = 70
        Top = 15
        Width = 436
        Height = 87
        Align = alClient
        BevelOuter = bvNone
        TabOrder = 1
        object edit_base_temp: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 3
          Top = 0
          Width = 430
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Base temperature'
          TabOrder = 0
        end
        object edit_opt_temp: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 3
          Top = 21
          Width = 430
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Optimal temperature'
          TabOrder = 1
        end
        object edit_max_temp: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 3
          Top = 42
          Width = 430
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Maximum temperature'
          TabOrder = 2
        end
        object edit_cutoff_temp: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 3
          Top = 63
          Width = 430
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Cutoff temperature'
          TabOrder = 3
        end
      end
    end
    object groupbox_transpiration: TGroupBox
      Left = 0
      Top = -1319
      Width = 508
      Height = 127
      Align = alTop
      Caption = 'Transpiration'
      TabOrder = 4
      object edit_max_water_uptake_mm: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 99
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Maximum water uptake'
        TabOrder = 0
      end
      object edit_wilt_leaf_water_pot: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 78
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Wilting leaf water potential'
        TabOrder = 1
      end
      object edit_leaf_water_pot_stomatal_closure: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 57
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Leaf water potential at the onset of stomatal closure'
        TabOrder = 2
      end
      object edit_ET_crop_coef: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 36
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Evapotranspiration crop coefficient at full canopy'
        TabOrder = 3
      end
      object edit_kc: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 15
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Canopy extinction coefficient for total (global) solar radiation'
        TabOrder = 4
      end
    end
    object groupbox_attainable_growth: TGroupBox
      Left = 0
      Top = 1644
      Width = 508
      Height = 531
      Align = alTop
      Caption = 'Attainable growth'
      TabOrder = 5
      object groupbox_transpiration_dependent_growth: TGroupBox
        Left = 2
        Top = 100
        Width = 504
        Height = 186
        Align = alTop
        Caption = 'Transpiration dependent growth'
        TabOrder = 0
        object checkbox_WUE_changes: TRNAutoCheckBox
          Left = 2
          Top = 15
          Width = 500
          Height = 17
          Align = alTop
          Caption = 
            'Water use efficiency may differ during vegetative growth and rep' +
            'roductive growth'
          State = cbGrayed
          TabOrder = 0
        end
        object edit_WUE_change_deg_day: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 35
          Width = 494
          Height = 21
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Thermal time for change of water use efficiency'
          TabOrder = 1
        end
        object radiogroup_WUE_submodel: TRNAutoRadioGroup
          Left = 2
          Top = 59
          Width = 500
          Height = 62
          Align = alTop
          Caption = 'Water use efficiency submodel'
          Items.Strings = (
            'Tanner && Sinclair'
            'FAO AquaCrop water productivity'
            'Transpiration Use Efficiency curve')
          TabOrder = 2
          OnClick = radiogroup_WUE_submodelClick
        end
        object scrollbox_WUE: TScrollBox
          Left = 2
          Top = 121
          Width = 500
          Height = 64
          Align = alTop
          BorderStyle = bsNone
          TabOrder = 3
          object groupbox_tanner_sinclair: TGroupBox
            Left = 0
            Top = 0
            Width = 483
            Height = 41
            Align = alTop
            Caption = 'Tanner && Sinclair'
            TabOrder = 0
            object edit_reference_biomass_transp_coef_0: TRNAutoParameterEditBar
              AlignWithMargins = True
              Left = 5
              Top = 18
              Width = 473
              Height = 21
              Hint = 'Biomass water ratio'
              Align = alTop
              Alignment = taLeftJustify
              BevelOuter = bvNone
              Caption = 'Above ground biomass transpiration coefficient (K) for annuals'
              TabOrder = 0
              object edit_reference_biomass_transp_coef_vegetative_0: TRNAutoFloatEdit
                Left = 301
                Top = 0
                Width = 70
                Height = 21
                Hint = 'Transpiration coefficient during vegetative growth'
                Align = alRight
                Alignment = taRightJustify
                Lines.Strings = (
                  '0')
                TabOrder = 5
              end
            end
          end
          object groupbox_aquacrop: TGroupBox
            Left = 0
            Top = 41
            Width = 483
            Height = 40
            Align = alTop
            Caption = 'FAO AquaCrop water productivity'
            TabOrder = 1
            object edit_water_productivity: TRNAutoParameterEditBar
              AlignWithMargins = True
              Left = 5
              Top = 18
              Width = 473
              Height = 21
              Hint = 
                'FAO AquaCrop water productivity during reproductive growth (or t' +
                'hroughout the season depending on the WUE mode).'
              Align = alTop
              Alignment = taLeftJustify
              BevelOuter = bvNone
              Caption = 'Water productivity'
              TabOrder = 0
              object edit_water_productivity_vegetative: TRNAutoFloatEdit
                Left = 301
                Top = 0
                Width = 70
                Height = 21
                Hint = 'Water productivity during vegetative growth'
                Align = alRight
                Alignment = taRightJustify
                Lines.Strings = (
                  '0')
                ParentShowHint = False
                ShowHint = True
                TabOrder = 5
              end
            end
          end
          object groupbox_TUE_curve: TGroupBox
            Left = 0
            Top = 81
            Width = 483
            Height = 64
            Align = alTop
            Caption = 'Transpiration Use Efficiency curve'
            TabOrder = 2
            object edit_TUE_scaling_coef: TRNAutoParameterEditBar
              AlignWithMargins = True
              Left = 5
              Top = 36
              Width = 473
              Height = 21
              Margins.Top = 0
              Margins.Bottom = 0
              Align = alTop
              Alignment = taLeftJustify
              BevelOuter = bvNone
              Caption = 'Scaling coefficient of TUE regression (power function)'
              TabOrder = 0
              object edit_TUE_scaling_coef_vegetative: TRNAutoFloatEdit
                Left = 301
                Top = 0
                Width = 70
                Height = 21
                Hint = 'Scaling coefficient of TUE regression during vegetative growth'
                Align = alRight
                Alignment = taRightJustify
                Lines.Strings = (
                  '0')
                TabOrder = 5
              end
            end
            object edit_TUE_at_1kPa_VPD: TRNAutoParameterEditBar
              AlignWithMargins = True
              Left = 5
              Top = 15
              Width = 473
              Height = 21
              Hint = 
                'Transpiration Use Efficiency during reproductive growth (or thro' +
                'ughout the season depending on the WUE mode).'
              Margins.Top = 0
              Margins.Bottom = 0
              Align = alTop
              Alignment = taLeftJustify
              BevelOuter = bvNone
              Caption = 'Transpiration Use Efficiency when VPD is at 1kPa'
              TabOrder = 1
              object edit_TUE_at_1kPa_VPD_vegetative: TRNAutoFloatEdit
                Left = 301
                Top = 0
                Width = 70
                Height = 21
                Hint = 'Transpiration use efficiency during vegetative growth'
                Align = alRight
                Alignment = taRightJustify
                Lines.Strings = (
                  '0')
                TabOrder = 5
              end
            end
          end
        end
      end
      object groupbox_seasonal_adjustment: TGroupBox
        Left = 2
        Top = 286
        Width = 504
        Height = 176
        Align = alTop
        Caption = 'Seasonal adjustment'
        TabOrder = 1
        object Label9: TLabel
          Left = 2
          Top = 15
          Width = 220
          Height = 13
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Caption = 'The first season (sowing year) is not adjusted'
        end
        object label_seasonal_adjustment: TLabel
          Left = 2
          Top = 133
          Width = 500
          Height = 42
          Align = alTop
          AutoSize = False
          Caption = 
            'In the case of perennials there may be a gradual change of bioma' +
            'ss production in successive seasons. Enter multiplier values bet' +
            'ween 0 and 1 to decrease growth, or values greater than 0 to inc' +
            'rease attainable growth.'
          Color = clInfoBk
          ParentColor = False
          Transparent = False
          WordWrap = True
          ExplicitWidth = 499
        end
        object edit_seasonal_adjustment_1: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 28
          Width = 494
          Height = 21
          Hint = 'Biomass adjustment'
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Season 1 (Not applicable season 1 is not adjusted)'
          TabOrder = 0
        end
        object edit_seasonal_adjustment_2: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 49
          Width = 494
          Height = 21
          Hint = 'Biomass adjustment season 2'
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Season 2'
          TabOrder = 1
        end
        object edit_seasonal_adjustment_3: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 70
          Width = 494
          Height = 21
          Hint = 'Biomass adjustment season 3'
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Season 3'
          TabOrder = 2
        end
        object edit_seasonal_adjustment_4: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 91
          Width = 494
          Height = 21
          Hint = 'Biomass adjustment season 4'
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Season 4'
          TabOrder = 3
        end
        object edit_seasonal_adjustment_5: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 112
          Width = 494
          Height = 21
          Hint = 'Biomass adjustment season 5 and any following seasons'
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Season 5 and all following seasons'
          TabOrder = 4
        end
      end
      object groupbox_radiation_dependent_growth: TGroupBox
        Left = 2
        Top = 15
        Width = 504
        Height = 85
        Align = alTop
        Caption = 'Radiation dependent growth'
        TabOrder = 2
        object label_must_not_be_less_than_base_temperature: TLabel
          Left = 2
          Top = 36
          Width = 478
          Height = 13
          Align = alTop
          Caption = 
            'Growth limiting temperature must not be less than the thermal ti' +
            'me accumulation base temperature.'
          Color = clInfoBk
          ParentColor = False
          Transparent = False
          WordWrap = True
        end
        object edit_opt_temp_for_growth: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 15
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Mean daily temperature that limits early growth'
          TabOrder = 0
        end
        object edit_RUE_global_basis: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 49
          Width = 494
          Height = 21
          Hint = 'about twice RUE PAR basis'
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 
            'Maximum Radiation Use Efficiency (unstressed, low vapor pressure' +
            ' deficit)'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
        end
      end
    end
    object groupbox_canopy: TGroupBox
      Left = 0
      Top = 1418
      Width = 508
      Height = 226
      Align = alTop
      Caption = 'Canopy growth'
      TabOrder = 6
      object radiogroup_canopy_growth: TRNAutoRadioGroup
        Left = 2
        Top = 15
        Width = 504
        Height = 33
        Align = alTop
        Caption = 'Model'
        Columns = 2
        Items.Strings = (
          'Leaf Area Index based'
          'Canopy cover based')
        TabOrder = 0
        OnClick = radiogroup_canopy_growthClick
      end
      object ScrollBox2: TScrollBox
        Left = 2
        Top = 48
        Width = 504
        Height = 134
        Align = alClient
        BevelEdges = []
        BevelInner = bvNone
        BevelOuter = bvNone
        BorderStyle = bsNone
        TabOrder = 1
        object panel_leaf_area_index_based: TPanel
          Left = 0
          Top = 152
          Width = 487
          Height = 129
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 0
          object edit_initial_GAI: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 0
            Width = 481
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Initial green leaf area index'
            TabOrder = 0
          end
          object edit_regrowth_GAI: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 21
            Width = 481
            Height = 21
            Hint = 
              'When there are clipping events or the crop comes out stress or d' +
              'ormancy conditions.this is the minimal green leaf area index ava' +
              'ilable to restart growth. Applies to perennial and/or clipped an' +
              'nuals'
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 
              'Min. green leaf area index for regrowth (perennial and/or clippe' +
              'd annuals)'
            TabOrder = 1
          end
          object edit_max_LAI: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 42
            Width = 481
            Height = 21
            Hint = 'For fruit tree crops, GAI will be limited to this value. For ot'
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Maximum expected leaf area index (required for fruit trees)'
            TabOrder = 2
          end
          object edit_specific_leaf_area: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 63
            Width = 481
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Specific leaf area at optimum temperature'
            TabOrder = 3
          end
          object edit_fract_LAI_mature: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 84
            Width = 481
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Fraction of max. LAI at physiological maturity'
            TabOrder = 4
          end
          object edit_stem_leaf_partition: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 105
            Width = 481
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Stem/leaf partition coefficient'
            TabOrder = 5
          end
        end
        object panel_canopy_cover_based: TPanel
          Left = 0
          Top = 0
          Width = 487
          Height = 152
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 1
          object edit_canopy_cover_initial: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 0
            Width = 481
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Initial canopy cover'
            TabOrder = 0
          end
          object edit_canopy_cover_maximum: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 21
            Width = 481
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Maximum canopy cover'
            TabOrder = 1
          end
          object edit_canopy_cover_green_mature: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 42
            Width = 481
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Green canopy cover at maturity'
            TabOrder = 2
          end
          object edit_canopy_cover_total_mature: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 63
            Width = 481
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Total canopy cover at maturity  (green and senesced)'
            TabOrder = 3
          end
          object edit_RTT_at_half_CCmax: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 84
            Width = 481
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Relative thermal time at half maximum canopy cover'
            TabOrder = 4
          end
          object edit_begin_senescence_deg_day: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 105
            Width = 481
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Begin senescence (thermal time)'
            TabOrder = 5
          end
        end
      end
      object edit_LWP_that_reduces_canopy_expansion: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 182
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alBottom
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Leaf water potential that begins reduction of canopy expansion'
        TabOrder = 2
      end
      object edit_LWP_that_stops_canopy_expansion: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 203
        Width = 498
        Height = 21
        Hint = 'Must be more negative than value above that begins reduction'
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alBottom
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Leaf water potential that stops canopy expansion'
        TabOrder = 3
      end
    end
    object groupbox_phenology: TGroupBox
      Left = 0
      Top = 1167
      Width = 508
      Height = 251
      Margins.Top = 0
      Margins.Bottom = 0
      Align = alTop
      Caption = 'Phenology'
      TabOrder = 7
      object Panel1: TPanel
        Left = 2
        Top = 15
        Width = 504
        Height = 25
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Thermal time to reach:'
        TabOrder = 0
        object checkbox_clipping_resets: TRNAutoCheckBox
          AlignWithMargins = True
          Left = 345
          Top = 3
          Width = 156
          Height = 19
          Hint = 
            'If enabled, clipping events will reset active growth stage (use ' +
            'for perennials, such as alfalfa, grass and other fodder crops)'
          Align = alRight
          Anchors = [akTop, akBottom]
          Caption = 'Clipping resets active growth'
          ParentShowHint = False
          ShowHint = True
          State = cbGrayed
          TabOrder = 0
        end
      end
      object edit_phenologic_sensitivity_water_stress: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 227
        Width = 498
        Height = 21
        Hint = '0 indicates no effect, 1 indicates full effect.'
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Adjustment factor for phenologic response to stress'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
      end
      object edit_deg_day_begin_maturity: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 206
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Physiological maturity'
        TabOrder = 2
      end
      object edit_deg_day_begin_rapid_fruit_growth: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 166
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Begin rapid fruit growth'
        TabOrder = 3
      end
      object edit_deg_day_begin_filling_grain_or_fruit: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 145
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 
          'Begin filling (seed filling, or orchard fruit filling, or tuber ' +
          'bulking)'
        TabOrder = 4
      end
      object edit_deg_day_begin_flower: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 124
        Width = 498
        Height = 21
        Hint = 'If this is not a flowering crop, enter a very large number.'
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Begin flowering'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 5
      end
      object edit_deg_day_end_vegetative_growth: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 103
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'End vegetative growth'
        TabOrder = 6
      end
      object edit_max_root_depth_dd: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 82
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Maximum root depth achieved'
        TabOrder = 7
      end
      object edit_deg_day_tuber_initiation: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 61
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Begin tuber initiation'
        TabOrder = 8
      end
      object edit_deg_day_emerge: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 40
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Emergence / Start of shoot growth'
        TabOrder = 9
      end
      object panel_maturity_significant: TPanel
        Left = 2
        Top = 187
        Width = 504
        Height = 19
        Align = alTop
        Alignment = taRightJustify
        BevelOuter = bvNone
        Caption = '(applicable to annual crop and fruit trees and not perennials)'
        TabOrder = 10
        object checkbox_maturity_significant: TRNAutoCheckBox
          Left = 0
          Top = 0
          Width = 156
          Height = 19
          Align = alLeft
          Caption = 'Maturity is significant'
          State = cbGrayed
          TabOrder = 0
          OnClick = checkbox_maturity_significantClick
        end
      end
    end
    object groupbox_vernalization: TGroupBox
      Left = 0
      Top = -1442
      Width = 508
      Height = 123
      Align = alTop
      TabOrder = 8
      DesignSize = (
        508
        123)
      object checkbox_vernalization: TRNAutoCheckBox
        Left = 5
        Top = 4
        Width = 500
        Height = 17
        Anchors = [akLeft, akTop, akRight]
        Caption = 'Vernalization'
        State = cbGrayed
        TabOrder = 0
        OnClick = checkbox_vernalizationClick
      end
      object edit_vern_end: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 78
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Vernalization days required to complete vernalization'
        TabOrder = 1
      end
      object edit_vern_start: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 57
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Vernalization days to enable vernalization '
        TabOrder = 2
      end
      object edit_vern_high_temp: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 36
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'High temperature for optimal vernalization'
        TabOrder = 3
      end
      object edit_vern_low_temp: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 15
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Low temperature for optimal vernalization'
        TabOrder = 4
      end
      object edit_vern_min_factor: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 99
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        AutoSize = True
        BevelOuter = bvNone
        Caption = 'Minimum vernalization factor'
        TabOrder = 5
      end
    end
    object groupbox_root: TGroupBox
      Left = 0
      Top = 84
      Width = 508
      Height = 129
      Align = alTop
      Caption = 'Root'
      TabOrder = 9
      object edit_at_pt_root_limit: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 99
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Act. to pot. transpiration ratio that limits root growth'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
      end
      object edit_root_density_distribution_curvature: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 78
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Curvature of root density distribution'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
      end
      object edit_surface_root_density: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 57
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Maximum surface root density'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
      end
      object edit_root_length_per_unit_mass: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 36
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Root length per unit root mass'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 3
      end
      object edit_max_root_depth: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 15
        Width = 498
        Height = 21
        Hint = 'Established root depth for established fruit trees'
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Maximum root depth'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 4
      end
    end
    object groupbox_harvest: TGroupBox
      Left = 0
      Top = 213
      Width = 508
      Height = 188
      Align = alTop
      Caption = 'Harvest'
      TabOrder = 10
      object TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 6
        Top = 148
        Width = 437
        Height = 23
        Margins.Top = 0
        Margins.Bottom = 0
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 
          'Sensitivity to water and nitrogen stress during active growth(ob' +
          'solete)'
        TabOrder = 0
      end
      object edit_HI_unstressed: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 15
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Unstressed harvest index'
        TabOrder = 1
      end
      object page_control_harvest: TPageControl
        Left = 2
        Top = 53
        Width = 504
        Height = 136
        ActivePage = tabsheet_harvest_grain
        Align = alTop
        TabOrder = 2
        object tabsheet_harvest_grain: TTabSheet
          Caption = 'Grain or seed'
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object groupbox_harvest_sensitivity: TGroupBox
            Left = 0
            Top = 27
            Width = 496
            Height = 41
            Align = alTop
            Caption = 'Sensitivity to water and nitrogen stress during'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 0
            object edit_HI_filling_sensitivity_obsolete: TRNAutoParameterEditBar
              AlignWithMargins = True
              Left = 140
              Top = -9
              Width = 272
              Height = 21
              Alignment = taLeftJustify
              BevelOuter = bvNone
              Caption = 'filling/bulking(obsolete)'
              TabOrder = 0
              Visible = False
            end
            object edit_HI_flowering_sensitivity: TRNAutoParameterEditBar
              AlignWithMargins = True
              Left = 5
              Top = 18
              Width = 486
              Height = 21
              Align = alTop
              Alignment = taLeftJustify
              BevelOuter = bvNone
              Caption = 'flowering'
              TabOrder = 1
            end
          end
          object edit_grain_translocation_fraction: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 3
            Width = 490
            Height = 21
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Translocation fraction'
            TabOrder = 1
          end
          object RNAutoParameterEditBar4_obsolete: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 26
            Top = 74
            Width = 445
            Height = 21
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'during grain filling (obsolete)'
            TabOrder = 2
          end
          object edit_HI_flowering_temperature_sensitivity: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 71
            Width = 490
            Height = 21
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Temperature stress sensitivity during flowering'
            TabOrder = 3
          end
          object edit_HI_filling_duration_obsolete: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 124
            Top = 84
            Width = 445
            Height = 21
            Hint = 'Intended for climate change scenarios.'
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Duration of grain filling period (unstressed) obsolete'
            TabOrder = 4
          end
        end
        object tabsheet_harvest_tuber: TTabSheet
          Caption = 'Tuber'
          ImageIndex = 1
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object Label6: TLabel
            Left = 0
            Top = 0
            Width = 157
            Height = 13
            Align = alTop
            Caption = 'Sensitivity to temperature stress'
          end
          object edit_HI_tuber_init_sensitivity: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 16
            Width = 490
            Height = 21
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'during tuber initiation'
            TabOrder = 0
          end
          object edit_HI_tuber_growth_sensitivity: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 43
            Width = 490
            Height = 21
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'during tuber growth'
            TabOrder = 1
          end
        end
        object tabsheet_harvest_fruit: TTabSheet
          Caption = 'Fruit'
          ImageIndex = 2
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object edit_harvest_temperature: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 0
            Width = 490
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Harvest temperature'
            TabOrder = 0
          end
          object edit_harvest_temperature_days: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 21
            Width = 490
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Harvest temperature days'
            TabOrder = 1
          end
          object groupbox_harvest_DOY: TGroupBox
            Left = 0
            Top = 42
            Width = 496
            Height = 62
            Align = alTop
            Caption = 'Harvest day of year'
            TabOrder = 2
            object edit_harvest_DOY_earliest: TRNAutoParameterEditBar
              AlignWithMargins = True
              Left = 5
              Top = 15
              Width = 486
              Height = 21
              Margins.Top = 0
              Margins.Bottom = 0
              Align = alTop
              Alignment = taLeftJustify
              BevelOuter = bvNone
              Caption = 'earliest'
              TabOrder = 0
            end
            object edit_harvest_DOY_latest: TRNAutoParameterEditBar
              AlignWithMargins = True
              Left = 5
              Top = 36
              Width = 486
              Height = 21
              Margins.Top = 0
              Margins.Bottom = 0
              Align = alTop
              Alignment = taLeftJustify
              BevelOuter = bvNone
              Caption = 'latest'
              TabOrder = 1
            end
          end
          object panel_harvest_fruit: TPanel
            Left = 140
            Top = 0
            Width = 229
            Height = 97
            Caption = 'panel_harvest_fruit'
            TabOrder = 3
            object radiogroup_fruit_harvest_condition: TRNAutoRadioGroup
              Left = 16
              Top = 1
              Width = 186
              Height = 127
              Caption = 'Harvest conditions'
              Items.Strings = (
                'Harvest is scheduled in management file'
                'Harvest on a fixed date'
                
                  'Harvest between specified dates considering minimum air temperat' +
                  'ure'
                
                  'Harvest between specified dates considering average air temperat' +
                  'ure')
              TabOrder = 0
            end
          end
        end
        object tabsheet_harvest_other: TTabSheet
          Caption = 'Other'
          ImageIndex = 3
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object edit_HI_growth_sensitivity: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 3
            Width = 445
            Height = 21
            Hint = 'Intended for climate change scenarios.'
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Sensitivity to water and nitrogen stress during active growth'
            TabOrder = 0
          end
        end
      end
      object checkbox_HI_adjustments: TRNAutoCheckBox
        Left = 2
        Top = 36
        Width = 504
        Height = 17
        Align = alTop
        Caption = 'Adjustments to unstressed harvest index'
        State = cbGrayed
        TabOrder = 3
        OnClick = checkbox_HI_adjustmentsClick
      end
    end
    object groupbox_dormancy: TGroupBox
      Left = 0
      Top = 781
      Width = 508
      Height = 347
      Align = alTop
      Caption = 'Dormancy/Seasonal'
      TabOrder = 11
      DesignSize = (
        508
        347)
      object pagecontrol_season_dormancy_mode: TPageControl
        Left = 2
        Top = 15
        Width = 504
        Height = 194
        ActivePage = tabsheet_dormancy
        Align = alClient
        Style = tsButtons
        TabOrder = 0
        OnChange = pagecontrol_season_dormancy_modeChange
        object tabsheet_single_season: TTabSheet
          Caption = 'Single season'
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object label_single_season_mode_1: TLabel
            Left = 0
            Top = 0
            Width = 496
            Height = 33
            Align = alTop
            AutoSize = False
            Caption = 
              'This mode is most commonly used for annual, biennial or (sometim' +
              'es) perennial crops that are not allowed to (or cannot) grow to ' +
              'a second season.  This mode is also usually used for winter crop' +
              's.'
            Color = clInfoBk
            ParentColor = False
            Transparent = False
            WordWrap = True
            ExplicitWidth = 538
          end
          object label_single_season_mode_2: TLabel
            Left = 0
            Top = 33
            Width = 496
            Height = 33
            Align = alTop
            AutoSize = False
            Caption = 
              'This mode is has no additional parameters and the crop tends to ' +
              'remain in maturity until harvest.'
            Color = clInfoBk
            ParentColor = False
            Transparent = False
            WordWrap = True
            ExplicitWidth = 538
          end
        end
        object tabsheet_multiple_season: TTabSheet
          Caption = 'Multiple season'
          ImageIndex = 1
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object label_multiple_season_mode_1: TLabel
            Left = 0
            Top = 0
            Width = 496
            Height = 33
            Align = alTop
            AutoSize = False
            Caption = 
              'This mode is use for biennial or perennial crops or plants that ' +
              'can propogate themselves and tend to persist for multiple season' +
              's (I.e. grasses).'
            Color = clInfoBk
            ParentColor = False
            Transparent = False
            WordWrap = True
            ExplicitWidth = 538
          end
          object label_multiple_season_mode_2: TLabel
            Left = 0
            Top = 33
            Width = 496
            Height = 33
            Align = alTop
            AutoSize = False
            Caption = 
              'Plants may go into a state of inactivity (but not necessarily do' +
              'rmancy) that typifies the end of the season.'
            Color = clInfoBk
            ParentColor = False
            Transparent = False
            WordWrap = True
            ExplicitWidth = 538
          end
        end
        object tabsheet_dormancy: TTabSheet
          Caption = 'Dormancy'
          ImageIndex = 2
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
          object label_dormancy_mode: TLabel
            Left = 0
            Top = 0
            Width = 496
            Height = 41
            Align = alTop
            AutoSize = False
            Caption = 
              'Dormancy is important for perennials, biannuals, and orchard/vin' +
              'eyard fruits, but may be applicable to certain other crops as we' +
              'll.  Plants will become inactive so the inactive period paramete' +
              'rs are significant.'
            Color = clInfoBk
            ParentColor = False
            Transparent = False
            WordWrap = True
            ExplicitWidth = 467
          end
          object groupbox_chill_requirements: TGroupBox
            Left = 0
            Top = 62
            Width = 496
            Height = 101
            Align = alClient
            Caption = 'Chill requirements for fruit trees'
            TabOrder = 0
            object edit_chill_threshold: TRNAutoParameterEditBar
              AlignWithMargins = True
              Left = 5
              Top = 78
              Width = 486
              Height = 21
              Margins.Top = 0
              Margins.Bottom = 0
              Align = alTop
              Alignment = taLeftJustify
              BevelOuter = bvNone
              Caption = 'Threshold temperature for chill requirement (not currently used)'
              TabOrder = 0
            end
            object edit_deg_day_bud_break_chill_req_not_sat: TRNAutoParameterEditBar
              AlignWithMargins = True
              Left = 5
              Top = 36
              Width = 486
              Height = 21
              Margins.Top = 0
              Margins.Bottom = 0
              Align = alTop
              Alignment = taLeftJustify
              BevelOuter = bvNone
              Caption = 'Thermal time to bud break (chill requirement not satisfied)'
              TabOrder = 1
            end
            object edit_deg_day_bud_break_chill_req_sat: TRNAutoParameterEditBar
              AlignWithMargins = True
              Left = 5
              Top = 15
              Width = 486
              Height = 21
              Margins.Top = 0
              Margins.Bottom = 0
              Align = alTop
              Alignment = taLeftJustify
              BevelOuter = bvNone
              Caption = 'Thermal time to bud break (chill requirement satisfied)'
              TabOrder = 2
            end
            object edit_chill_requirement: TRNAutoParameterEditBar
              AlignWithMargins = True
              Left = 5
              Top = 57
              Width = 486
              Height = 21
              Margins.Top = 0
              Margins.Bottom = 0
              Align = alTop
              Alignment = taLeftJustify
              BevelOuter = bvNone
              Caption = 'Chill requirement'
              TabOrder = 3
            end
          end
          object edit_dormancy_senesce: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 41
            Width = 490
            Height = 21
            Hint = 
              'For perennial crops that hold on to canopy biomass through winte' +
              'r.  Use 100% if there is full leaf drop.'
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Biomass that senesces at the onset of dormancy'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 1
          end
        end
      end
      object groupbox_inactive_period: TGroupBox
        Left = 2
        Top = 209
        Width = 504
        Height = 136
        Align = alBottom
        Caption = 'Inactive period'
        TabOrder = 1
        object label_inactivity_days_note: TLabel
          Left = 2
          Top = 15
          Width = 490
          Height = 26
          Align = alTop
          Caption = 
            'Number of days of consecutive of average daily temperature below' +
            ' the inducement temperature that identifies'
          WordWrap = True
        end
        object edit_consider_inactive_days: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 41
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'at end or restart of growth activity'
          TabOrder = 0
        end
        object edit_inducement_temperature: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 62
          Width = 494
          Height = 21
          Hint = 
            'Average temperature over these consecutive days below which inac' +
            'tivity is induced'
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'edit_inducement_temperature'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
        end
        object panel_leave_valid_date: TPanel
          Left = 2
          Top = 83
          Width = 500
          Height = 25
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 
            'First date to start looking for restart after inactive period (s' +
            'tart of season)'
          TabOrder = 2
          object button_start_valid_date: TRNAutoDateButton
            Left = 425
            Top = 0
            Width = 75
            Height = 25
            Align = alRight
            Caption = 'No Date'
            TabOrder = 0
          end
        end
        object panel_start_valid_date: TPanel
          Left = 2
          Top = 108
          Width = 500
          Height = 25
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'First date to start looking for inactive period (end of season)'
          TabOrder = 3
          object button_leave_valid_date: TRNAutoDateButton
            Left = 425
            Top = 0
            Width = 75
            Height = 25
            Align = alRight
            Caption = 'No Date'
            TabOrder = 0
          end
        end
      end
      object Edit1: TEdit
        AlignWithMargins = True
        Left = 288
        Top = 15
        Width = 206
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        BorderStyle = bsNone
        Color = clInfoBk
        TabOrder = 2
        Text = '<- Select one of these modes'
      end
    end
    object groupbox_senescence: TGroupBox
      Left = 0
      Top = -100
      Width = 508
      Height = 184
      Align = alTop
      Caption = 'Senescence'
      TabOrder = 12
      DesignSize = (
        508
        184)
      object label_senescence_perennial: TLabel
        Left = 239
        Top = 16
        Width = 258
        Height = 13
        ParentCustomHint = False
        Anchors = [akTop, akRight]
        Caption = '(note that perennials may now have leaf senescence)'
        ExplicitLeft = 230
      end
      object checkbox_senesces: TRNAutoCheckBox
        Left = 2
        Top = 15
        Width = 504
        Height = 17
        Align = alTop
        Anchors = [akTop, akRight]
        Caption = 'Senesces'
        State = cbGrayed
        TabOrder = 0
      end
      object groupbox_LAI_duration: TGroupBox
        Left = 2
        Top = 32
        Width = 504
        Height = 65
        Align = alTop
        Caption = 'LAI duration (for LAI based canopy growth)'
        TabOrder = 1
        object edit_leaf_duration_sensitivity_stress: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 36
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Leaf area duration sensitivity to water stress'
          TabOrder = 0
        end
        object edit_deg_day_leaf_duration: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 15
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Leaf area duration'
          TabOrder = 1
        end
      end
      object radiogroup_absission: TRNAutoRadioGroup
        Left = 2
        Top = 97
        Width = 504
        Height = 56
        Hint = 'What happens to biomass as it dies on the plant'
        Align = alTop
        Caption = 'Absicission'
        Columns = 3
        Items.Strings = (
          'none'
          'evergreen'
          'semi-evergreen'
          'marcescent'
          'semi-deciduous'
          'deciduous')
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
        OnClick = radiogroup_absissionClick
      end
      object edit_linger: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 156
        Width = 498
        Height = 21
        Hint = 'remain standing/attached to'
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Time that senesced leaves linger with the plant'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 3
      end
    end
    object groupbox_hardiness: TGroupBox
      Left = 0
      Top = -648
      Width = 508
      Height = 113
      Align = alTop
      Caption = 'Hardiness'
      TabOrder = 13
      object checkbox_sensitive_to_cold: TRNAutoCheckBox
        Left = 2
        Top = 15
        Width = 504
        Height = 17
        Align = alTop
        Caption = 'Sensitive to cold temperatures'
        State = cbGrayed
        TabOrder = 0
        OnClick = checkbox_sensitive_to_coldClick
      end
      object groupbox_cold_temperature_sensitivity: TGroupBox
        Left = 2
        Top = 32
        Width = 504
        Height = 81
        Align = alTop
        Caption = 'Cold temperature sensitivity'
        TabOrder = 1
        DesignSize = (
          504
          81)
        object Label3: TLabel
          Left = 335
          Top = 0
          Width = 31
          Height = 13
          Caption = 'leaves'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlue
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
        end
        object edit_damage_cold_temperature: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 15
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Cold temperature which begins to damage the plant'
          TabOrder = 1
        end
        object edit_lethal_cold_temperature: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 36
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Cold temperature which is lethal to the plant'
          TabOrder = 2
        end
        object edit_salvageable_days_before_maturity: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 57
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Number of days before maturity to salvage yield'
          TabOrder = 3
        end
        object groupbox_hardiness_fruit: TGroupBox
          Left = 261
          Top = 0
          Width = 73
          Height = 62
          Anchors = [akTop, akRight]
          Caption = 'flower/fruit'
          TabOrder = 0
          object edit_hardiness_fruit_damage: TRNAutoFloatEdit
            Left = 2
            Top = 15
            Width = 69
            Height = 21
            Align = alTop
            Alignment = taRightJustify
            Lines.Strings = (
              '0')
            TabOrder = 0
          end
          object edit_hardiness_fruit_lethal: TRNAutoFloatEdit
            Left = 2
            Top = 36
            Width = 69
            Height = 21
            Align = alTop
            Alignment = taRightJustify
            Lines.Strings = (
              '0')
            TabOrder = 1
          end
        end
      end
    end
    object groupbox_residue: TGroupBox
      Left = 0
      Top = -535
      Width = 508
      Height = 242
      Align = alTop
      Caption = 'Residue'
      TabOrder = 14
      object panel_residue_1: TPanel
        Left = 2
        Top = 15
        Width = 504
        Height = 82
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 0
        object radiogroup_RULSE2_residue_type: TRNAutoRadioGroup
          Left = 0
          Top = 0
          Width = 187
          Height = 82
          Align = alLeft
          Caption = 'RUSLE2 residue type'
          Items.Strings = (
            'unknown (defaults to moderate)'
            'fragile/very small (I.e.soybeans)'
            'mod. tough/short (I.e.wheat)'
            'nonfragile/med. (I.e. corn)'
            'woody/large')
          TabOrder = 0
        end
        object residue_panel_1A: TPanel
          Left = 187
          Top = 0
          Width = 317
          Height = 82
          Align = alClient
          BevelOuter = bvNone
          TabOrder = 1
          object edit_stubble_area_covered_to_mass_ratio: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 21
            Width = 311
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Stubble area covered to mass ratio'
            TabOrder = 0
          end
          object edit_carbon_fraction: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 0
            Width = 311
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Top and root carbon fraction'
            TabOrder = 1
          end
          object edit_surface_area_covered_to_mass_ratio: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 42
            Width = 311
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Flattened surface residue area covered to mass ratio'
            TabOrder = 2
          end
          object edit_detrition_time: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 3
            Top = 63
            Width = 311
            Height = 21
            Hint = 
              'Accounts for disintigration as a result of physical processes su' +
              'ch as wind damage, abrasion, photodegradation, etc..'
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Days to 50% detrition'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 3
          end
        end
      end
      object panel_residue_2: TPanel
        Left = 2
        Top = 97
        Width = 504
        Height = 144
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 1
        object Label1: TLabel
          Left = 4
          Top = 38
          Width = 59
          Height = 13
          Caption = 'Top biomass'
        end
        object Label4: TLabel
          Left = 4
          Top = 62
          Width = 64
          Height = 13
          Caption = 'Root biomass'
        end
        object Label14: TLabel
          Left = 172
          Top = 6
          Width = 56
          Height = 13
          Caption = 'Fast cycling'
        end
        object Label18: TLabel
          Left = 196
          Top = 22
          Width = 24
          Height = 13
          Caption = '(0-1)'
        end
        object Label15: TLabel
          Left = 244
          Top = 6
          Width = 57
          Height = 13
          Caption = 'Slow cycling'
        end
        object Label19: TLabel
          Left = 268
          Top = 22
          Width = 24
          Height = 13
          Caption = '(0-1)'
        end
        object Label16: TLabel
          Left = 307
          Top = 6
          Width = 80
          Height = 13
          Caption = 'Lignified biomass'
        end
        object Label20: TLabel
          Left = 340
          Top = 22
          Width = 24
          Height = 13
          Caption = '(0-1)'
        end
        object Label21: TLabel
          Left = 400
          Top = 22
          Width = 48
          Height = 13
          Caption = 'Checksum'
        end
        object edit_fast_cycling_top_fraction: TRNAutoFloatEdit
          Left = 168
          Top = 40
          Width = 71
          Height = 24
          Alignment = taRightJustify
          Lines.Strings = (
            '0')
          TabOrder = 0
          OnExit = update_top_biomass_checksum
        end
        object edit_slow_cycling_top_fraction: TRNAutoFloatEdit
          Left = 238
          Top = 40
          Width = 70
          Height = 24
          Alignment = taRightJustify
          Lines.Strings = (
            '0')
          TabOrder = 1
          OnExit = update_top_biomass_checksum
        end
        object edit_lignified_top_fraction: TRNAutoFloatEdit
          Left = 308
          Top = 40
          Width = 70
          Height = 24
          Alignment = taRightJustify
          Lines.Strings = (
            '0')
          TabOrder = 2
          OnExit = update_top_biomass_checksum
        end
        object top_biomass_checksum: TRNAutoFloatEdit
          Left = 392
          Top = 40
          Width = 70
          Height = 24
          Alignment = taRightJustify
          Lines.Strings = (
            '0')
          TabOrder = 3
        end
        object edit_fast_cycling_root_fraction: TRNAutoFloatEdit
          Left = 168
          Top = 64
          Width = 70
          Height = 24
          Alignment = taRightJustify
          Lines.Strings = (
            '0')
          TabOrder = 4
          OnExit = update_root_biomass_checksum
        end
        object edit_slow_cycling_root_fraction: TRNAutoFloatEdit
          Left = 238
          Top = 64
          Width = 70
          Height = 24
          Alignment = taRightJustify
          Lines.Strings = (
            '0')
          TabOrder = 5
          OnExit = update_root_biomass_checksum
        end
        object edit_lignified_root_fraction: TRNAutoFloatEdit
          Left = 308
          Top = 64
          Width = 70
          Height = 24
          Alignment = taRightJustify
          Lines.Strings = (
            '0')
          TabOrder = 6
          OnExit = update_root_biomass_checksum
        end
        object root_biomass_checksum: TRNAutoFloatEdit
          Left = 392
          Top = 64
          Width = 70
          Height = 24
          Alignment = taRightJustify
          Lines.Strings = (
            '0')
          TabOrder = 7
        end
        object panel_residue_advanced: TPanel
          Left = 0
          Top = 88
          Width = 485
          Height = 25
          BevelOuter = bvNone
          TabOrder = 8
          object Label7: TLabel
            Left = 4
            Top = 0
            Width = 153
            Height = 31
            AutoSize = False
            Caption = 'Number of days to reach 50% decomposition (days)'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clMaroon
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = []
            ParentFont = False
            WordWrap = True
          end
          object edit_fast_cycling_decomposition_time_50: TRNAutoFloatEdit
            Left = 168
            Top = 0
            Width = 70
            Height = 24
            Alignment = taRightJustify
            Lines.Strings = (
              '0')
            TabOrder = 0
          end
          object edit_slow_cycling_decomposition_time_50: TRNAutoFloatEdit
            Left = 238
            Top = 0
            Width = 70
            Height = 24
            Alignment = taRightJustify
            Lines.Strings = (
              '0')
            TabOrder = 1
          end
          object edit_lignified_decomposition_time_50: TRNAutoFloatEdit
            Left = 308
            Top = 0
            Width = 70
            Height = 24
            Alignment = taRightJustify
            Lines.Strings = (
              '0')
            TabOrder = 2
          end
        end
        object edit_fast_cycling_C_N_ratio: TRNAutoFloatEdit
          Left = 168
          Top = 118
          Width = 71
          Height = 24
          Alignment = taRightJustify
          Lines.Strings = (
            '0')
          TabOrder = 9
          Visible = False
        end
        object edit_slow_cycling_C_N_ratio: TRNAutoFloatEdit
          Left = 238
          Top = 118
          Width = 70
          Height = 24
          Alignment = taRightJustify
          Lines.Strings = (
            '0')
          TabOrder = 10
          Visible = False
        end
        object edit_lignified_C_N_ratio: TRNAutoFloatEdit
          Left = 308
          Top = 118
          Width = 70
          Height = 24
          Alignment = taRightJustify
          Lines.Strings = (
            '0')
          TabOrder = 11
          Visible = False
        end
      end
    end
    object groupbox_nitrogen: TGroupBox
      Left = 0
      Top = -1071
      Width = 508
      Height = 262
      Align = alTop
      Caption = 'Nitrogen'
      TabOrder = 15
      object groupbox_nitrogen_demand: TGroupBox
        Left = 2
        Top = 15
        Width = 504
        Height = 81
        Align = alTop
        Caption = 'Demand'
        TabOrder = 0
        DesignSize = (
          504
          81)
        object edit_demand_adjust: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 15
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Nitrogen demand adjustment'
          TabOrder = 0
        end
        object edit_N_max_stubble: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 36
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Maximum N concentration of chaff and stubble'
          TabOrder = 1
        end
        object edit_root_N_conc: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 57
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Standard root N concentration'
          TabOrder = 2
        end
        object button_nitrogen_fixation: TRNAutoCheckBox
          Left = 144
          Top = -2
          Width = 357
          Height = 17
          Anchors = [akLeft, akTop, akRight]
          Caption = 'Nitrogen fixation (I.e. for legumes)'
          TabOrder = 3
        end
      end
      object groupbox_nitrogen_uptake: TGroupBox
        Left = 2
        Top = 96
        Width = 504
        Height = 104
        Align = alTop
        Caption = 'Uptake'
        TabOrder = 1
        object edit_PAW_where_N_uptake_rate_decreases: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 78
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Plant available water at which N uptake starts decreasing'
          TabOrder = 0
        end
        object edit_residual_N_not_available_for_uptake: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 36
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Residual N not available for uptake'
          TabOrder = 1
        end
        object edit_N_max_daily_uptake_rate: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 15
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Maximum uptake during rapid linear growth'
          TabOrder = 2
        end
        object edit_soil_N_conc_where_N_uptake_decreases: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 57
          Width = 494
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Soil N concentration at which N uptake starts decreasing'
          TabOrder = 3
        end
      end
      object radiogroup_nitrogen_leaf_stress_mode: TRNAutoRadioGroup
        Left = 2
        Top = 200
        Width = 504
        Height = 60
        Align = alClient
        Caption = 
          'Leaf expansion is reduced when canopy N concentration falls belo' +
          'w '
        Items.Strings = (
          'No effect of nitrogen stress (as prior to version 4.07.05)'
          'the critical level with full stress at the minimum level'
          
            'the midpoint between  the maximum and critical levels with full ' +
            'stress at the midpoint between the critical and minimum levels')
        TabOrder = 2
      end
    end
    object groupbox_salinity: TGroupBox
      Left = 0
      Top = -188
      Width = 508
      Height = 88
      Align = alTop
      Caption = 'Salinity'
      TabOrder = 16
      object Label29: TLabel
        Left = 2
        Top = 57
        Width = 504
        Height = 32
        Align = alTop
        AutoSize = False
        Caption = 
          'These parameters are optional.  They are not used unless the sal' +
          'inity simulation submodel is enabled (in the scenario model opti' +
          'ons)'
        Color = clInfoBk
        ParentColor = False
        Transparent = False
        WordWrap = True
        ExplicitWidth = 496
      end
      object edit_salt_tolerance_P: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 36
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Salinity tolerance exponent (Van-Genuchten p)'
        TabOrder = 0
      end
      object edit_osmotic_pot_50: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 15
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Soil solution osmotic potential for 50% yield reduction'
        TabOrder = 1
      end
    end
    object groupbox_CO2: TGroupBox
      Left = 0
      Top = -293
      Width = 508
      Height = 105
      Align = alTop
      Caption = 'CO2'
      TabOrder = 17
      object Label28: TLabel
        Left = 2
        Top = 78
        Width = 504
        Height = 27
        Align = alTop
        AutoSize = False
        Caption = 
          'These parameters are optional.  They are not used unless the CO2' +
          ' simulation submodel is enabled (in the scenario model options)'
        Color = clInfoBk
        ParentColor = False
        Transparent = False
        WordWrap = True
        ExplicitWidth = 496
      end
      object edit_CO2_baseline_reference_conc: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 57
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Elevated to baseline growth ratio'
        TabOrder = 0
      end
      object edit_CO2_elevated_reference_conc: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 36
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Elevated reference atmospheric CO2 concentration'
        TabOrder = 1
      end
      object edit_CO2_growth_ratio_elevated_to_baseline: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 15
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Baseline reference atmospheric CO2 concentration'
        TabOrder = 2
      end
    end
    object groupbox_fruit: TGroupBox
      Left = 0
      Top = -809
      Width = 508
      Height = 161
      Align = alTop
      Caption = 'Fruit'
      TabOrder = 18
      object edit_initial_reserves_release_rate: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 78
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Initial release rate of biomass reserves (per day)'
        TabOrder = 0
      end
      object edit_initial_fruit_mass: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 57
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Initial fruit mass'
        TabOrder = 1
      end
      object edit_fruit_fract_total_solids: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 36
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Fraction of fruit dry matter'
        TabOrder = 2
      end
      object edit_fruit_load_max: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 15
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Maximum fruit load (fresh mass)'
        TabOrder = 3
      end
      object panel_grape_or_other: TPanel
        Left = 2
        Top = 99
        Width = 504
        Height = 62
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 4
        object groupbox_fruit_grape: TGroupBox
          Left = 2
          Top = 123
          Width = 466
          Height = 61
          Caption = 'Grape'
          TabOrder = 0
          object edit_clusters_per_vine: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 5
            Top = 15
            Width = 456
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Clusters per vine'
            TabOrder = 0
          end
          object edit_berries_per_cluster: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 5
            Top = 36
            Width = 456
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Berries per cluster'
            TabOrder = 1
          end
        end
        object groupbox_grape: TGroupBox
          Left = 267
          Top = 0
          Width = 237
          Height = 62
          Align = alClient
          Caption = 'Grape'
          TabOrder = 2
          object RNAutoParameterEditBar5: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 5
            Top = 15
            Width = 227
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Clusters per vine'
            TabOrder = 0
          end
          object RNAutoParameterEditBar6: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 5
            Top = 36
            Width = 227
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Berries per cluster'
            TabOrder = 1
          end
        end
        object groupbox_fruit_other: TGroupBox
          Left = 0
          Top = 0
          Width = 267
          Height = 62
          Align = alLeft
          Caption = 'Pome, Drupe, Hesperidium etc..'
          TabOrder = 1
          object edit_fruits_per_tree: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 5
            Top = 15
            Width = 257
            Height = 21
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Fruits per tree'
            TabOrder = 0
          end
        end
      end
    end
    object groupbox_CROPGRO: TGroupBox
      Left = 0
      Top = 1128
      Width = 508
      Height = 39
      Align = alTop
      Caption = 'CROPGRO'
      TabOrder = 19
    end
    object groupbox_photoperiod: TGroupBox
      Left = 0
      Top = -1549
      Width = 508
      Height = 107
      Align = alTop
      Caption = 'Photo-period'
      TabOrder = 20
      object radiogroup_photoperiod_consideration: TRNAutoRadioGroup
        Left = 2
        Top = 15
        Width = 110
        Height = 90
        Align = alLeft
        Caption = 'Consideration'
        Items.Strings = (
          'none'
          'Short-day crop'
          'Long-day crop')
        TabOrder = 0
        OnClick = radiogroup_photoperiod_considerationClick
      end
      object groupbox_photoperiod_daylength: TGroupBox
        Left = 112
        Top = 15
        Width = 394
        Height = 90
        Align = alClient
        Caption = 'Day length affects flowering development'
        TabOrder = 1
        object caption_at_which_stopped: TLabel
          Left = 2
          Top = 15
          Width = 114
          Height = 13
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Caption = 'day length above which'
        end
        object caption_at_which_constrained: TLabel
          Left = 2
          Top = 49
          Width = 112
          Height = 13
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Caption = 'day length below which'
        end
        object edit_stop_daylength: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 28
          Width = 384
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'development is stopped'
          TabOrder = 0
        end
        object edit_constrained_daylength: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 62
          Width = 384
          Height = 21
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'development is not constrained'
          TabOrder = 1
        end
      end
    end
    object radiogroup_emergence_model: TRNAutoRadioGroup
      Left = 0
      Top = 601
      Width = 508
      Height = 34
      Align = alTop
      Caption = 'Emergence submodel'
      Columns = 2
      Items.Strings = (
        'Thermal time'
        'Hydrothermal time')
      TabOrder = 21
      OnClick = radiogroup_emergence_modelClick
    end
    object groupbox_max_water_uptake: TGroupBox
      Left = 0
      Top = -1192
      Width = 508
      Height = 121
      Align = alTop
      Caption = 'Maximum water uptake'
      TabOrder = 22
      object edit_max_water_upake_before_fruit: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 15
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'before fruiting'
        TabOrder = 0
      end
      object edit_max_water_uptake_during_intial_fruit: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 36
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'during initial fruit filling'
        TabOrder = 1
      end
      object edit_max_water_uptake_during_rapid_fruit: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 57
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'during rapid fruit filling'
        TabOrder = 2
      end
      object edit_max_water_uptake_during_veraison: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 78
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'during veraison (if applicable)'
        TabOrder = 3
      end
      object edit_max_water_uptake_after_maturity: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 5
        Top = 99
        Width = 498
        Height = 21
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'after maturity'
        TabOrder = 4
      end
    end
  end
  object groupbox_jumpto: TAdvGroupBox
    Left = 0
    Top = 0
    Width = 177
    Height = 659
    Align = alLeft
    Caption = 'Show/Jump to'
    TabOrder = 1
    object scrollbox_jumpto: TScrollBox
      Left = 2
      Top = 16
      Width = 173
      Height = 641
      Align = alClient
      BevelInner = bvNone
      BevelOuter = bvNone
      BorderStyle = bsNone
      TabOrder = 0
      object radiogroup_crop_model: TRNAutoRadioGroup
        Left = 0
        Top = 290
        Width = 173
        Height = 71
        Align = alTop
        Caption = 'Crop model'
        Items.Strings = (
          'CropSyst generalized crops (grain, root, and leaf)'
          
            'CropSyst orchard/vineyard fruit model (fruit trees, stone fruit,' +
            ' grapes)'
          'CROPGRO (Peas, beans, soy, legumes) (not implemented)')
        TabOrder = 0
        OnClick = radiogroup_crop_modelClick
      end
      object checklistbox_view_panels: TRNAutoBitCheckListBox
        Left = 0
        Top = 0
        Width = 173
        Height = 290
        OnClickCheck = checklistbox_view_panelsClickCheck
        Align = alTop
        ItemHeight = 13
        Items.Strings = (
          'Defaults'
          'Classification'
          'Thermal time accumulation'
          'Transpiration'
          'Vernalization'
          'Photo-period'
          'Emergence'
          'Phenology'
          'Canopy growth'
          'Attainable growth'
          'Senescence'
          'Dormacy/Seasonal'
          'Root'
          'Fruit'
          'Layout'
          'Harvest'
          'Hardiness'
          'Residue'
          'Nitrogen'
          'Salinity'
          'CO2'
          'CROPGRO')
        TabOrder = 1
        OnClick = checklistbox_view_panelsClick
      end
      object groupbox_defaults: TGroupBox
        Left = 0
        Top = 361
        Width = 173
        Height = 270
        Align = alTop
        Caption = 'Defaults'
        TabOrder = 2
        object Label2: TLabel
          Left = 1
          Top = 61
          Width = 171
          Height = 60
          Align = alTop
          AutoSize = False
          Caption = 
            'To load default values, select the crop type.  All current param' +
            'eter settings will be updated.'
          Color = clInfoBk
          ParentColor = False
          WordWrap = True
          ExplicitWidth = 156
        end
        object Label24: TLabel
          Left = 1
          Top = 14
          Width = 171
          Height = 26
          Align = alTop
          AutoSize = False
          Caption = 'Default crop parameters are available for some crop types.'
          Color = clInfoBk
          ParentColor = False
          WordWrap = True
          ExplicitLeft = 8
          ExplicitTop = 15
          ExplicitWidth = 550
        end
        object Label25: TLabel
          Left = 1
          Top = 146
          Width = 165
          Height = 91
          Align = alTop
          Caption = 
            'This button will launch the crop calibration utility to calibrat' +
            'e parameters for  location/cultivar. First select the default cr' +
            'op type above.  Note that crop calibration is currently only ava' +
            'ilable for annual crops.'
          Color = clInfoBk
          ParentColor = False
          WordWrap = True
        end
        object crop_class_combobox: TComboBox
          Left = 1
          Top = 40
          Width = 171
          Height = 21
          Align = alTop
          TabOrder = 0
          Text = '<default crop not currently selected>'
        end
        object button_calibrate: TButton
          Left = 1
          Top = 121
          Width = 171
          Height = 25
          Hint = 'This will open'
          Align = alTop
          Caption = 'Calibrate crop parameters'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'MS Sans Serif'
          Font.Style = []
          ParentFont = False
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          Visible = False
          OnClick = button_calibrateClick
        end
      end
    end
  end
end
