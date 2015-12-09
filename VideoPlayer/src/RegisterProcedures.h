//
//  RegisterProcedures.h
//  VideoPlayer
//
//  Created by Yi Huang on 24/6/15.
//  Copyright (c) 2015 huangyi. All rights reserved.
//

#if !defined ( _REGISTER_PROCEDURES_H_ )
#define _REGISTER_PROCEDURES_H_

#include "PlayerProcedure.h"

pipeline_procedure_desc* RegisterProcedure( pipeline_builder* builder, const ProcedureDesc& desc, void* context );

void UnregisterProcedure( pipeline_builder* builder, const ProcedureDesc& desc );

bool RegisterAll( pipeline_builder* builder, void* context );

void UnregisterAll( pipeline_builder* builder );

#endif /* defined(_REGISTER_PROCEDURES_H_) */
